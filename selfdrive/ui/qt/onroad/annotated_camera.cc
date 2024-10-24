#include "selfdrive/ui/qt/onroad/annotated_camera.h"

#include <QPainter>
#include <algorithm>
#include <cmath>

#include "common/swaglog.h"
#include "selfdrive/ui/qt/onroad/buttons.h"
#include "selfdrive/ui/qt/util.h"

// Window that shows camera view and variety of info drawn on top
AnnotatedCameraWidget::AnnotatedCameraWidget(VisionStreamType type, QWidget* parent) : fps_filter(UI_FREQ, 3, 1. / UI_FREQ), CameraWidget("camerad", type, true, parent) {
  pm = std::make_unique<PubMaster, const std::initializer_list<const char *>>({"uiDebug"});

  main_layout = new QVBoxLayout(this);
  main_layout->setMargin(UI_BORDER_SIZE);
  main_layout->setSpacing(0);

  QHBoxLayout *buttons_layout = new QHBoxLayout();
  buttons_layout->setSpacing(0);

  // Neokii screen recorder
  screenRecorder = new ScreenRecorder(this);
  buttons_layout->addWidget(screenRecorder);

  experimental_btn = new ExperimentalButton(this);
  buttons_layout->addWidget(experimental_btn);

  QVBoxLayout *top_right_layout = new QVBoxLayout();
  top_right_layout->setSpacing(0);
  top_right_layout->addLayout(buttons_layout);

  pedal_icons = new PedalIcons(this);
  top_right_layout->addWidget(pedal_icons, 0, Qt::AlignRight);

  main_layout->addLayout(top_right_layout, 0);
  main_layout->setAlignment(top_right_layout, Qt::AlignTop | Qt::AlignRight);

  map_settings_btn = new MapSettingsButton(this);
  main_layout->addWidget(map_settings_btn, 0, Qt::AlignBottom | Qt::AlignRight);

  dm_img = loadPixmap("../assets/img_driver_face.png", {img_size + 5, img_size + 5});

  // Initialize FrogPilot widgets
  initializeFrogPilotWidgets();
}

void AnnotatedCameraWidget::updateState(int alert_height, const UIState &s) {
  const int SET_SPEED_NA = 255;
  const SubMaster &sm = *(s.sm);

  const bool cs_alive = sm.alive("controlsState");
  const bool nav_alive = sm.alive("navInstruction") && sm["navInstruction"].getValid();
  const auto cs = sm["controlsState"].getControlsState();
  const auto car_state = sm["carState"].getCarState();
  const auto nav_instruction = sm["navInstruction"].getNavInstruction();

  // Handle older routes where vCruiseCluster is not set
  float v_cruise = cs.getVCruiseCluster() == 0.0 ? cs.getVCruise() : cs.getVCruiseCluster();
  setSpeed = cs_alive ? v_cruise : SET_SPEED_NA;
  is_cruise_set = setSpeed > 0 && (int)setSpeed != SET_SPEED_NA;
  if (is_cruise_set && !s.scene.is_metric) {
    setSpeed *= KM_TO_MILE;
  }

  // Handle older routes where vEgoCluster is not set
  v_ego_cluster_seen = v_ego_cluster_seen || car_state.getVEgoCluster() != 0.0;
  float v_ego = v_ego_cluster_seen && !s.scene.wheel_speed ? car_state.getVEgoCluster() : car_state.getVEgo();
  speed = cs_alive ? std::max<float>(0.0, v_ego) : 0.0;
  speed *= s.scene.is_metric ? MS_TO_KPH : MS_TO_MPH;

  auto speed_limit_sign = nav_instruction.getSpeedLimitSign();
  speedLimit = slcOverridden ? s.scene.speed_limit_overridden_speed : speedLimitController ? s.scene.speed_limit : nav_alive ? nav_instruction.getSpeedLimit() : 0.0;
  speedLimit *= (s.scene.is_metric ? MS_TO_KPH : MS_TO_MPH);
  if (speedLimitController && !slcOverridden) {
    speedLimit = speedLimit - (showSLCOffset ? slcSpeedLimitOffset : 0);
  }

  has_us_speed_limit = (nav_alive && speed_limit_sign == cereal::NavInstruction::SpeedLimitSign::MUTCD) || (speedLimitController && !useViennaSLCSign);
  has_eu_speed_limit = (nav_alive && speed_limit_sign == cereal::NavInstruction::SpeedLimitSign::VIENNA) && !(speedLimitController && !useViennaSLCSign) || (speedLimitController && useViennaSLCSign);
  is_metric = s.scene.is_metric;
  speedUnit =  s.scene.is_metric ? tr("km/h") : tr("mph");
  hideBottomIcons = (cs.getAlertSize() != cereal::ControlsState::AlertSize::NONE || turnSignalAnimation && (turnSignalLeft || turnSignalRight) && (signalStyle == "traditional" || signalStyle == "traditional_gif") || bigMapOpen);
  status = s.status;

  // update engageability/experimental mode button
  experimental_btn->updateState(s, leadInfo);

  // update DM icon
  auto dm_state = sm["driverMonitoringState"].getDriverMonitoringState();
  dmActive = dm_state.getIsActiveMode();
  rightHandDM = dm_state.getIsRHD();
  // DM icon transition
  dm_fade_state = std::clamp(dm_fade_state+0.2*(0.5-dmActive), 0.0, 1.0);

  // hide map settings button for alerts and flip for right hand DM
  if (map_settings_btn->isEnabled()) {
    map_settings_btn->setVisible(!hideBottomIcons && compass && !hideMapIcon);
    main_layout->setAlignment(map_settings_btn, (rightHandDM && !compass || !rightHandDM && compass ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignBottom);
  }

  // Update FrogPilot widgets
  updateFrogPilotVariables(alert_height, s.scene);
}

void AnnotatedCameraWidget::drawHud(QPainter &p) {
  p.save();

  // Header gradient
  QLinearGradient bg(0, UI_HEADER_HEIGHT - (UI_HEADER_HEIGHT / 2.5), 0, UI_HEADER_HEIGHT);
  bg.setColorAt(0, QColor::fromRgbF(0, 0, 0, 0.45));
  bg.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
  p.fillRect(0, 0, width(), UI_HEADER_HEIGHT, bg);

  QString speedLimitStr = (speedLimit > 1) ? QString::number(std::nearbyint(speedLimit)) : "–";
  QString speedLimitOffsetStr = slcSpeedLimitOffset == 0 ? "–" : QString::number(slcSpeedLimitOffset, 'f', 0).prepend(slcSpeedLimitOffset > 0 ? "+" : "");
  QString speedStr = QString::number(std::nearbyint(speed));
  QString setSpeedStr = is_cruise_set ? QString::number(std::nearbyint(setSpeed - cruiseAdjustment)) : "–";

  if (!hideMaxSpeed) {
    // Draw outer box + border to contain set speed and speed limit
    const int sign_margin = 12;
    const int us_sign_height = 186;
    const int eu_sign_size = 176;

    const QSize default_size = {172, 204};
    QSize set_speed_size = default_size;
    if (is_metric || has_eu_speed_limit) set_speed_size.rwidth() = 200;
    if (has_us_speed_limit && speedLimitStr.size() >= 3) set_speed_size.rwidth() = 223;

    if (has_us_speed_limit) set_speed_size.rheight() += us_sign_height + sign_margin;
    else if (has_eu_speed_limit) set_speed_size.rheight() += eu_sign_size + sign_margin;

    int top_radius = 32;
    int bottom_radius = has_eu_speed_limit ? 100 : 32;

    QRect set_speed_rect(QPoint(60 + (default_size.width() - set_speed_size.width()) / 2, 45), set_speed_size);
    if (is_cruise_set && cruiseAdjustment != 0) {
      float transition = qBound(0.0f, 5.0f * (cruiseAdjustment / setSpeed), 1.0f);
      QColor min = whiteColor(75);
      QColor max = vtscControllingCurve ? redColor() : greenColor();

      p.setPen(QPen(QColor::fromRgbF(
        min.redF() + transition * (max.redF() - min.redF()),
        min.greenF() + transition * (max.greenF() - min.greenF()),
        min.blueF() + transition * (max.blueF() - min.blueF())
      ), 10));
    } else if (trafficModeActive) {
      p.setPen(QPen(redColor(), 10));
    } else if (reverseCruise) {
      p.setPen(QPen(blueColor(), 6));
    } else {
      p.setPen(QPen(whiteColor(75), 6));
    }
    p.setBrush(blackColor(166));
    drawRoundedRect(p, set_speed_rect, top_radius, top_radius, bottom_radius, bottom_radius);

    // Draw MAX
    QColor max_color = QColor(0x80, 0xd8, 0xa6, 0xff);
    QColor set_speed_color = whiteColor();
    if (is_cruise_set) {
      if (status == STATUS_DISENGAGED) {
        max_color = whiteColor();
      } else if (status == STATUS_OVERRIDE) {
        max_color = QColor(0x91, 0x9b, 0x95, 0xff);
      } else if (speedLimit > 0) {
        auto interp_color = [=](QColor c1, QColor c2, QColor c3) {
          return speedLimit > 0 ? interpColor(setSpeed, {speedLimit + 5, speedLimit + 15, speedLimit + 25}, {c1, c2, c3}) : c1;
        };
        max_color = interp_color(max_color, QColor(0xff, 0xe4, 0xbf), QColor(0xff, 0xbf, 0xbf));
        set_speed_color = interp_color(set_speed_color, QColor(0xff, 0x95, 0x00), QColor(0xff, 0x00, 0x00));
      }
    } else {
      max_color = QColor(0xa6, 0xa6, 0xa6, 0xff);
      set_speed_color = QColor(0x72, 0x72, 0x72, 0xff);
    }
    p.setFont(InterFont(40, QFont::DemiBold));
    p.setPen(max_color);
    p.drawText(set_speed_rect.adjusted(0, 27, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("MAX"));
    p.setFont(InterFont(90, QFont::Bold));
    p.setPen(set_speed_color);
    p.drawText(set_speed_rect.adjusted(0, 77, 0, 0), Qt::AlignTop | Qt::AlignHCenter, setSpeedStr);

    const QRect sign_rect = set_speed_rect.adjusted(sign_margin, default_size.height(), -sign_margin, -sign_margin);
    p.save();
    // US/Canada (MUTCD style) sign
    if (has_us_speed_limit) {
      p.setPen(Qt::NoPen);
      p.setBrush(whiteColor());
      p.drawRoundedRect(sign_rect, 24, 24);
      p.setPen(QPen(blackColor(), 6));
      p.drawRoundedRect(sign_rect.adjusted(9, 9, -9, -9), 16, 16);

      p.setOpacity(slcOverridden ? 0.25 : 1.0);
      if (speedLimitController && showSLCOffset && !slcOverridden) {
        p.setFont(InterFont(28, QFont::DemiBold));
        p.drawText(sign_rect.adjusted(0, 22, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("LIMIT"));
        p.setFont(InterFont(70, QFont::Bold));
        p.drawText(sign_rect.adjusted(0, 51, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitStr);
        p.setFont(InterFont(50, QFont::DemiBold));
        p.drawText(sign_rect.adjusted(0, 120, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitOffsetStr);
      } else {
        p.setFont(InterFont(28, QFont::DemiBold));
        p.drawText(sign_rect.adjusted(0, 22, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("SPEED"));
        p.drawText(sign_rect.adjusted(0, 51, 0, 0), Qt::AlignTop | Qt::AlignHCenter, tr("LIMIT"));
        p.setFont(InterFont(70, QFont::Bold));
        p.drawText(sign_rect.adjusted(0, 85, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitStr);
      }
    }

    // EU (Vienna style) sign
    if (has_eu_speed_limit) {
      p.setPen(Qt::NoPen);
      p.setBrush(whiteColor());
      p.drawEllipse(sign_rect);
      p.setPen(QPen(Qt::red, 20));
      p.drawEllipse(sign_rect.adjusted(16, 16, -16, -16));

      p.setOpacity(slcOverridden ? 0.25 : 1.0);
      p.setPen(blackColor());
      if (showSLCOffset) {
        p.setFont(InterFont((speedLimitStr.size() >= 3) ? 60 : 70, QFont::Bold));
        p.drawText(sign_rect.adjusted(0, -25, 0, 0), Qt::AlignCenter, speedLimitStr);
        p.setFont(InterFont(40, QFont::DemiBold));
        p.drawText(sign_rect.adjusted(0, 100, 0, 0), Qt::AlignTop | Qt::AlignHCenter, speedLimitOffsetStr);
      } else {
        p.setFont(InterFont((speedLimitStr.size() >= 3) ? 60 : 70, QFont::Bold));
        p.drawText(sign_rect, Qt::AlignCenter, speedLimitStr);
      }
    }
    p.restore();
  }

  // current speed
  if (!(bigMapOpen || hideSpeed)) {
    if (standstillDuration != 0) {
      float transition = qBound(0.0f, standstillDuration / 120.0f, 1.0f);
      QColor start, end;

      if (standstillDuration <= 60) {
        start = end = bg_colors[STATUS_ENGAGED];
      } else if (standstillDuration <= 90) {
        start = bg_colors[STATUS_ENGAGED];
        end = bg_colors[STATUS_CONDITIONAL_OVERRIDDEN];
        transition = (standstillDuration - 60) / 30.0f;
      } else if (standstillDuration <= 120) {
        start = bg_colors[STATUS_CONDITIONAL_OVERRIDDEN];
        end = bg_colors[STATUS_TRAFFIC_MODE_ACTIVE];
        transition = (standstillDuration - 90) / 30.0f;
      } else {
        start = end = bg_colors[STATUS_TRAFFIC_MODE_ACTIVE];
        transition = 0.0f;
      }

      float red = start.redF() + transition * (end.redF() - start.redF());
      float green = start.greenF() + transition * (end.greenF() - start.greenF());
      float blue = start.blueF() + transition * (end.blueF() - start.blueF());

      p.setPen(QPen(QColor::fromRgbF(red, green, blue)));

      int minutes = standstillDuration / 60;
      int seconds = standstillDuration % 60;

      p.setFont(InterFont(176, QFont::Bold));
      drawText(p, rect().center().x(), 210, minutes == 1 ? "1 minute" : QString("%1 minutes").arg(minutes), 255, true);
      p.setFont(InterFont(66));
      drawText(p, rect().center().x(), 290, QString("%1 seconds").arg(seconds));
    } else {
      p.setFont(InterFont(176, QFont::Bold));
      drawText(p, rect().center().x(), 210, speedStr);
      p.setFont(InterFont(66));
      drawText(p, rect().center().x(), 290, speedUnit, 200);
    }
  }

  p.restore();
}

void AnnotatedCameraWidget::drawText(QPainter &p, int x, int y, const QString &text, int alpha, bool overridePen) {
  QRect real_rect = p.fontMetrics().boundingRect(text);
  real_rect.moveCenter({x, y - real_rect.height() / 2});

  if (!overridePen) {
    p.setPen(QColor(0xff, 0xff, 0xff, alpha));
  }
  p.drawText(real_rect.x(), real_rect.bottom(), text);
}

void AnnotatedCameraWidget::initializeGL() {
  CameraWidget::initializeGL();
  qInfo() << "OpenGL version:" << QString((const char*)glGetString(GL_VERSION));
  qInfo() << "OpenGL vendor:" << QString((const char*)glGetString(GL_VENDOR));
  qInfo() << "OpenGL renderer:" << QString((const char*)glGetString(GL_RENDERER));
  qInfo() << "OpenGL language version:" << QString((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

  prev_draw_t = millis_since_boot();
  setBackgroundColor(bg_colors[STATUS_DISENGAGED]);
}

void AnnotatedCameraWidget::updateFrameMat() {
  CameraWidget::updateFrameMat();
  UIState *s = uiState();
  int w = width(), h = height();

  s->fb_w = w;
  s->fb_h = h;

  // Apply transformation such that video pixel coordinates match video
  // 1) Put (0, 0) in the middle of the video
  // 2) Apply same scaling as video
  // 3) Put (0, 0) in top left corner of video
  s->car_space_transform.reset();
  s->car_space_transform.translate(w / 2 - x_offset, h / 2 - y_offset)
      .scale(zoom, zoom)
      .translate(-intrinsic_matrix.v[2], -intrinsic_matrix.v[5]);
}

void AnnotatedCameraWidget::drawLaneLines(QPainter &painter, const UIState *s, float v_ego) {
  painter.save();

  const UIScene &scene = s->scene;
  SubMaster &sm = *(s->sm);

  // lanelines
  for (int i = 0; i < std::size(scene.lane_line_vertices); ++i) {
    if (useStockColors) {
      painter.setBrush(QColor::fromRgbF(1.0, 1.0, 1.0, std::clamp<float>(scene.lane_line_probs[i], 0.0, 0.7)));
    } else {
      painter.setBrush(scene.lane_lines_color);
    }
    painter.drawPolygon(scene.lane_line_vertices[i]);
  }

  // road edges
  for (int i = 0; i < std::size(scene.road_edge_vertices); ++i) {
    if (useStockColors) {
      painter.setBrush(QColor::fromRgbF(1.0, 0, 0, std::clamp<float>(1.0 - scene.road_edge_stds[i], 0.0, 1.0)));
    } else {
      painter.setBrush(scene.road_edges_color);
    }
    painter.drawPolygon(scene.road_edge_vertices[i]);
  }

  // paint path
  QLinearGradient bg(0, height(), 0, 0);
  if (experimentalMode || scene.acceleration_path) {
    // The first half of track_vertices are the points for the right side of the path
    // and the indices match the positions of accel from uiPlan
    const auto &acceleration_const = sm["uiPlan"].getUiPlan().getAccel();
    const int max_len = std::min<int>(scene.track_vertices.length() / 2, acceleration_const.size());

    // Copy of the acceleration vector
    std::vector<float> acceleration;
    acceleration.reserve(acceleration_const.size());
    for (size_t i = 0; i < acceleration_const.size(); ++i) {
      acceleration.push_back(acceleration_const[i]);
    }

    for (int i = 0; i < max_len; ++i) {
      // Some points are out of frame
      int track_idx = max_len - i - 1;  // flip idx to start from bottom right
      if (scene.track_vertices[track_idx].y() < 0 || scene.track_vertices[track_idx].y() > height()) continue;

      // Flip so 0 is bottom of frame
      float lin_grad_point = (height() - scene.track_vertices[track_idx].y()) / height();

      // If acceleration is between -0.25 and 0.25, resort to the theme color
      if (std::abs(acceleration[i]) < 0.25 && !useStockColors) {
        QColor color = scene.path_color;

        bg.setColorAt(0.0, color);

        color.setAlphaF(0.5);
        bg.setColorAt(0.5, color);

        color.setAlphaF(0.1);
        bg.setColorAt(1.0, color);
      } else {
        // speed up: 120, slow down: 0
        float path_hue = fmax(fmin(60 + acceleration[i] * 35, 120), 0);
        // FIXME: painter.drawPolygon can be slow if hue is not rounded
        path_hue = int(path_hue * 100 + 0.5) / 100;

        float saturation = fmin(fabs(acceleration[i] * 1.5), 1);
        float lightness = util::map_val(saturation, 0.0f, 1.0f, 0.95f, 0.62f);  // lighter when grey
        float alpha = util::map_val(lin_grad_point, 0.75f / 2.f, 0.75f, 0.4f, 0.0f);  // matches previous alpha fade
        bg.setColorAt(lin_grad_point, QColor::fromHslF(path_hue / 360., saturation, lightness, alpha));

        // Skip a point, unless next is last
        i += (i + 2) < max_len ? 1 : 0;
      }
    }

  } else {
    bg.setColorAt(0.0, QColor::fromHslF(148 / 360., 0.94, 0.51, 0.4));
    bg.setColorAt(0.5, QColor::fromHslF(112 / 360., 1.0, 0.68, 0.35));
    bg.setColorAt(1.0, QColor::fromHslF(112 / 360., 1.0, 0.68, 0.0));
  }

  painter.setBrush(bg);
  painter.drawPolygon(scene.track_vertices);

  if (scene.show_stopping_point && scene.red_light && speed > 1 && !(conditionalStatus == 1 || conditionalStatus == 3 || conditionalStatus == 5)) {
    QPointF last_point = scene.track_vertices.last();

    QPointF adjusted_point = last_point - QPointF(stopSignImg.width() / 2, stopSignImg.height());
    painter.drawPixmap(adjusted_point, stopSignImg);

    if (scene.show_stopping_point_metrics) {
      QString text = QString::number(modelLength * distanceConversion) + leadDistanceUnit;
      QFont font = InterFont(35, QFont::DemiBold);
      QFontMetrics fm(font);
      int text_width = fm.horizontalAdvance(text);
      QPointF text_position = last_point - QPointF(text_width / 2, stopSignImg.height() + 35);

      painter.save();
      painter.setFont(font);
      painter.setPen(Qt::white);
      painter.drawText(text_position, text);
      painter.restore();
    }
  }

  // Paint blindspot path
  if (scene.blind_spot_path) {
    QLinearGradient bs(0, height(), 0, 0);

    bs.setColorAt(0.0, QColor::fromHslF(0 / 360., 0.75, 0.50, 0.6));
    bs.setColorAt(0.5, QColor::fromHslF(0 / 360., 0.75, 0.50, 0.4));
    bs.setColorAt(1.0, QColor::fromHslF(0 / 360., 0.75, 0.50, 0.2));

    painter.setBrush(bs);
    if (blindSpotLeft) {
      painter.drawPolygon(scene.track_adjacent_vertices[4]);
    }
    if (blindSpotRight) {
      painter.drawPolygon(scene.track_adjacent_vertices[5]);
    }
  }

  // Paint adjacent lane paths
  if ((scene.adjacent_path || scene.adjacent_path_metrics) && v_ego > scene.minimum_lane_change_speed) {
    const float minLaneWidth = laneDetectionWidth * 0.5f;
    const float maxLaneWidth = laneDetectionWidth * 1.5f;

    auto paintLane = [&](const QPolygonF &lane, float laneWidth, bool blindspot) {
      QLinearGradient gradient(0, height(), 0, 0);

      bool redPath = laneWidth < minLaneWidth || laneWidth > maxLaneWidth || blindspot;
      float hue = redPath ? 0.0f : 120.0f * (laneWidth - minLaneWidth) / (maxLaneWidth - minLaneWidth);
      float hueF = hue / 360.0f;

      gradient.setColorAt(0.0, QColor::fromHslF(hueF, 0.75f, 0.50f, 0.6f));
      gradient.setColorAt(0.5, QColor::fromHslF(hueF, 0.75f, 0.50f, 0.4f));
      gradient.setColorAt(1.0, QColor::fromHslF(hueF, 0.75f, 0.50f, 0.2f));

      painter.setBrush(gradient);
      painter.drawPolygon(lane);

      if (scene.adjacent_path_metrics) {
        painter.setFont(InterFont(30, QFont::DemiBold));
        painter.setPen(Qt::white);

        QRectF boundingRect = lane.boundingRect();
        QString text = blindspot ? tr("Vehicle in blind spot") : QString::number(laneWidth * distanceConversion, 'f', 2) + leadDistanceUnit;
        painter.drawText(boundingRect, Qt::AlignCenter, text);

        painter.setPen(Qt::NoPen);
      }
    };

    paintLane(scene.track_adjacent_vertices[4], scene.lane_width_left, blindSpotLeft);
    paintLane(scene.track_adjacent_vertices[5], scene.lane_width_right, blindSpotRight);
  }

  // Paint path edges
  QLinearGradient pe(0, height(), 0, 0);
  auto setGradientColors = [&](const QColor &baseColor) {
    pe.setColorAt(0.0, baseColor);
    QColor color = baseColor;
    color.setAlphaF(0.5);
    pe.setColorAt(0.5, color);
    color.setAlphaF(0.1);
    pe.setColorAt(1.0, color);
  };

  if (alwaysOnLateralActive) {
    setGradientColors(bg_colors[STATUS_ALWAYS_ON_LATERAL_ACTIVE]);
  } else if (conditionalStatus == 1 || conditionalStatus == 3 || conditionalStatus == 5) {
    setGradientColors(bg_colors[STATUS_CONDITIONAL_OVERRIDDEN]);
  } else if (experimentalMode) {
    setGradientColors(bg_colors[STATUS_EXPERIMENTAL_MODE_ACTIVE]);
  } else if (trafficModeActive) {
    setGradientColors(bg_colors[STATUS_TRAFFIC_MODE_ACTIVE]);
  } else if (scene.navigate_on_openpilot) {
    setGradientColors(bg_colors[STATUS_NAVIGATION_ACTIVE]);
  } else if (!useStockColors) {
    setGradientColors(scene.path_edges_color);
  } else {
    pe.setColorAt(0.0, QColor::fromHslF(148 / 360., 0.94, 0.51, 1.0));
    pe.setColorAt(0.5, QColor::fromHslF(112 / 360., 1.00, 0.68, 0.5));
    pe.setColorAt(1.0, QColor::fromHslF(112 / 360., 1.00, 0.68, 0.1));
  }

  QPainterPath path;
  path.addPolygon(scene.track_vertices);
  path.addPolygon(scene.track_edge_vertices);

  painter.setBrush(pe);
  painter.drawPath(path);

  painter.restore();
}

void AnnotatedCameraWidget::drawDriverState(QPainter &painter, const UIState *s) {
  const UIScene &scene = s->scene;

  painter.save();

  // base icon
  int offset = UI_BORDER_SIZE + btn_size / 2;
  int x = rightHandDM ? width() - offset : offset;
  if (rightHandDM && map_settings_btn->isEnabled() && !hideMapIcon) {
    x -= 250;
  } else if (onroadDistanceButton) {
    x += 250;
  }
  offset += statusBarHeight / 2;
  int y = height() - offset;
  float opacity = dmActive ? 0.65 : 0.2;
  drawIcon(painter, QPoint(x, y), dm_img, blackColor(70), opacity);

  // face
  QPointF face_kpts_draw[std::size(default_face_kpts_3d)];
  float kp;
  for (int i = 0; i < std::size(default_face_kpts_3d); ++i) {
    kp = (scene.face_kpts_draw[i].v[2] - 8) / 120 + 1.0;
    face_kpts_draw[i] = QPointF(scene.face_kpts_draw[i].v[0] * kp + x, scene.face_kpts_draw[i].v[1] * kp + y);
  }

  painter.setPen(QPen(QColor::fromRgbF(1.0, 1.0, 1.0, opacity), 5.2, Qt::SolidLine, Qt::RoundCap));
  painter.drawPolyline(face_kpts_draw, std::size(default_face_kpts_3d));

  // tracking arcs
  const int arc_l = 133;
  const float arc_t_default = 6.7;
  const float arc_t_extend = 12.0;
  QColor arc_color = QColor::fromRgbF(0.545 - 0.445 * s->engaged(),
                                      0.545 + 0.4 * s->engaged(),
                                      0.545 - 0.285 * s->engaged(),
                                      0.4 * (1.0 - dm_fade_state));
  float delta_x = -scene.driver_pose_sins[1] * arc_l / 2;
  float delta_y = -scene.driver_pose_sins[0] * arc_l / 2;
  painter.setPen(QPen(arc_color, arc_t_default+arc_t_extend*fmin(1.0, scene.driver_pose_diff[1] * 5.0), Qt::SolidLine, Qt::RoundCap));
  painter.drawArc(QRectF(std::fmin(x + delta_x, x), y - arc_l / 2, fabs(delta_x), arc_l), (scene.driver_pose_sins[1]>0 ? 90 : -90) * 16, 180 * 16);
  painter.setPen(QPen(arc_color, arc_t_default+arc_t_extend*fmin(1.0, scene.driver_pose_diff[0] * 5.0), Qt::SolidLine, Qt::RoundCap));
  painter.drawArc(QRectF(x - arc_l / 2, std::fmin(y + delta_y, y), arc_l, fabs(delta_y)), (scene.driver_pose_sins[0]>0 ? 0 : 180) * 16, 180 * 16);

  painter.restore();
}

void AnnotatedCameraWidget::drawLead(QPainter &painter, const cereal::RadarState::LeadData::Reader &lead_data, const QPointF &vd, float v_ego, const QColor &lead_marker_color, bool adjacent) {
  painter.save();

  const float speedBuff = useStockColors || adjacent ? 10. : 25.;  // Make the center of the chevron appear sooner if a theme is active
  const float leadBuff = useStockColors || adjacent ? 40. : 100.;  // Make the center of the chevron appear sooner if a theme is active
  const float d_rel = lead_data.getDRel() + (adjacent ? fabs(lead_data.getYRel()) : 0);
  const float v_rel = lead_data.getVRel();

  float fillAlpha = 0;
  if (d_rel < leadBuff) {
    fillAlpha = 255 * (1.0 - (d_rel / leadBuff));
    if (v_rel < 0) {
      fillAlpha += 255 * (-1 * (v_rel / speedBuff));
    }
    fillAlpha = (int)(fmin(fillAlpha, 255));
  }

  float sz = std::clamp((25 * 30) / (d_rel / 3 + 30), adjacent ? 10.0f : 15.0f, adjacent ? 20.0f : 30.0f) * 2.35;
  float x = std::clamp((float)vd.x(), 0.f, width() - sz / 2);
  float y = std::fmin(height() - sz * .6, (float)vd.y());

  float g_xo = sz / 5;
  float g_yo = sz / 10;

  QPointF glow[] = {{x + (sz * 1.35) + g_xo, y + sz + g_yo}, {x, y - g_yo}, {x - (sz * 1.35) - g_xo, y + sz + g_yo}};
  painter.setBrush(QColor(218, 202, 37, 255));
  painter.drawPolygon(glow, std::size(glow));

  // chevron
  QPointF chevron[] = {{x + (sz * 1.25), y + sz}, {x, y}, {x - (sz * 1.25), y + sz}};
  if (useStockColors) {
    painter.setBrush(redColor(fillAlpha));
  } else {
    painter.setBrush(lead_marker_color);
  }
  painter.drawPolygon(chevron, std::size(chevron));

  if (leadInfo) {
    float lead_speed = std::max(v_rel + v_ego, 0.0f);

    painter.setPen(Qt::white);
    painter.setFont(InterFont(35, QFont::Bold));

    QString text;
    if (adjacent) {
      text = QString("%1 %2 | %3 %4")
              .arg(qRound(d_rel * distanceConversion))
              .arg(leadDistanceUnit)
              .arg(qRound(lead_speed * speedConversion))
              .arg(leadSpeedUnit);
    } else {
      text = QString("%1 %2 | %3 %4 | %5 %6")
              .arg(qRound(d_rel * distanceConversion))
              .arg(leadDistanceUnit)
              .arg(qRound(lead_speed * speedConversion))
              .arg(leadSpeedUnit)
              .arg(QString::number(d_rel / std::max(v_ego, 1.0f), 'f', 1))
              .arg("s");
    }

    QFontMetrics metrics(painter.font());
    int middle_x = (chevron[2].x() + chevron[0].x()) / 2;
    int textWidth = metrics.horizontalAdvance(text);
    int text_x = middle_x - textWidth / 2;
    int text_y = chevron[0].y() + metrics.height() + 5;

    painter.drawText(text_x, text_y, text);
  }

  painter.restore();
}

void AnnotatedCameraWidget::paintGL() {
}

void AnnotatedCameraWidget::paintEvent(QPaintEvent *event) {
  UIState *s = uiState();
  SubMaster &sm = *(s->sm);
  QPainter painter(this);
  const double start_draw_t = millis_since_boot();
  const cereal::ModelDataV2::Reader &model = sm["modelV2"].getModelV2();
  const float v_ego = sm["carState"].getCarState().getVEgo();

  // draw camera frame
  {
    std::lock_guard lk(frame_lock);

    if (frames.empty()) {
      if (skip_frame_count > 0) {
        skip_frame_count--;
        qDebug() << "skipping frame, not ready";
        return;
      }
    } else {
      // skip drawing up to this many frames if we're
      // missing camera frames. this smooths out the
      // transitions from the narrow and wide cameras
      skip_frame_count = 5;
    }

    // Wide or narrow cam dependent on speed
    bool has_wide_cam = available_streams.count(VISION_STREAM_WIDE_ROAD);
    if (has_wide_cam && cameraView == 0) {
      if ((v_ego < 10) || available_streams.size() == 1) {
        wide_cam_requested = true;
      } else if (v_ego > 15) {
        wide_cam_requested = false;
      }
      wide_cam_requested = wide_cam_requested && experimentalMode;
      // for replay of old routes, never go to widecam
      wide_cam_requested = wide_cam_requested && s->scene.calibration_wide_valid;
    }
    CameraWidget::setStreamType(cameraView == 1 ? VISION_STREAM_DRIVER :
                                cameraView == 3 || wide_cam_requested ? VISION_STREAM_WIDE_ROAD :
                                VISION_STREAM_ROAD);

    s->scene.wide_cam = CameraWidget::getStreamType() == VISION_STREAM_WIDE_ROAD;
    if (s->scene.calibration_valid) {
      auto calib = s->scene.wide_cam ? s->scene.view_from_wide_calib : s->scene.view_from_calib;
      CameraWidget::updateCalibration(calib);
    } else {
      CameraWidget::updateCalibration(DEFAULT_CALIBRATION);
    }
    painter.beginNativePainting();
    CameraWidget::setFrameId(model.getFrameId());
    CameraWidget::paintGL();
    painter.endNativePainting();
  }

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);

  if (s->scene.world_objects_visible) {
    update_model(s, model, sm["uiPlan"].getUiPlan());
    drawLaneLines(painter, s, v_ego);

    if (sm.rcv_frame("radarState") > s->scene.started_frame && !s->scene.hide_lead_marker) {
      auto radar_state = sm["radarState"].getRadarState();
      update_leads(s, radar_state, model.getPosition());
      auto lead_one = radar_state.getLeadOne();
      auto lead_two = radar_state.getLeadTwo();
      auto lead_left = radar_state.getLeadLeft();
      auto lead_right = radar_state.getLeadRight();
      auto lead_left_far = radar_state.getLeadLeftFar();
      auto lead_right_far = radar_state.getLeadRightFar();
      if (lead_one.getStatus()) {
        drawLead(painter, lead_one, s->scene.lead_vertices[0], v_ego, s->scene.lead_marker_color);
      }
      if (lead_two.getStatus()) {
        drawLead(painter, lead_two, s->scene.lead_vertices[1], v_ego, s->scene.lead_marker_color);
      }
      if (lead_left.getStatus()) {
        drawLead(painter, lead_left, s->scene.lead_vertices[2], v_ego, blueColor(), true);
      }
      if (lead_right.getStatus()) {
        drawLead(painter, lead_right, s->scene.lead_vertices[3], v_ego, redColor(), true);
      }
      if (lead_left_far.getStatus()) {
        drawLead(painter, lead_left_far, s->scene.lead_vertices[4], v_ego, greenColor(), true);
      }
      if (lead_right_far.getStatus()) {
        drawLead(painter, lead_right_far, s->scene.lead_vertices[5], v_ego, whiteColor(), true);
      }
    }
  }

  // DMoji
  if (!hideBottomIcons && (sm.rcv_frame("driverStateV2") > s->scene.started_frame)) {
    update_dmonitoring(s, sm["driverStateV2"].getDriverStateV2(), dm_fade_state, rightHandDM);
    drawDriverState(painter, s);
  }

  drawHud(painter);

  double cur_draw_t = millis_since_boot();
  double dt = cur_draw_t - prev_draw_t;
  double fps = fps_filter.update(1. / dt * 1000);
  s->scene.fps = fps;
  if (fps < 15) {
    LOGW("slow frame rate: %.2f fps", fps);
  }
  prev_draw_t = cur_draw_t;

  // publish debug msg
  MessageBuilder msg;
  auto m = msg.initEvent().initUiDebug();
  m.setDrawTimeMillis(cur_draw_t - start_draw_t);
  pm->send("uiDebug", msg);

  // Paint FrogPilot widgets
  paintFrogPilotWidgets(painter);
}

void AnnotatedCameraWidget::showEvent(QShowEvent *event) {
  CameraWidget::showEvent(event);

  ui_update_params(uiState());
  prev_draw_t = millis_since_boot();

  // Update FrogPilot images
  distance_btn->updateIcon();
  experimental_btn->updateIcon();
  updateSignals();
}

// FrogPilot widgets
void AnnotatedCameraWidget::updateSignals() {
  blindspotImages.clear();
  signalImages.clear();

  QDir directory("../frogpilot/assets/active_theme/signals/");
  QFileInfoList allFiles = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

  bool isGif = false;
  for (QFileInfo &fileInfo : allFiles) {
    if (fileInfo.fileName().endsWith(".gif", Qt::CaseInsensitive)) {
      QMovie movie(fileInfo.absoluteFilePath());
      movie.start();

      for (int frameIndex = 0; frameIndex < movie.frameCount(); ++frameIndex) {
        movie.jumpToFrame(frameIndex);
        QPixmap currentFrame = movie.currentPixmap();
        signalImages.push_back(currentFrame);
        signalImages.push_back(currentFrame.transformed(QTransform().scale(-1, 1)));
      }

      movie.stop();
      isGif = true;

    } else if (fileInfo.fileName().endsWith(".png", Qt::CaseInsensitive)) {
      QVector<QPixmap> *targetList = fileInfo.fileName().contains("blindspot") ? &blindspotImages : &signalImages;
      QPixmap pixmap(fileInfo.absoluteFilePath());
      targetList->push_back(pixmap);
      targetList->push_back(pixmap.transformed(QTransform().scale(-1, 1)));

    } else {
      QStringList parts = fileInfo.fileName().split('_');
      if (parts.size() == 2) {
        signalStyle = parts[0];
        signalAnimationLength = parts[1].toInt();
      }
    }
  }

  if (!signalImages.empty()) {
    QPixmap &firstImage = signalImages.front();
    signalWidth = firstImage.width();
    signalHeight = firstImage.height();
    totalFrames = signalImages.size() / 2;
    turnSignalAnimation = true;

    if (isGif && signalStyle == "traditional") {
      signalMovement = (this->size().width() + (signalWidth * 2)) / totalFrames;
      signalStyle = "traditional_gif";
    } else {
      signalMovement = 0;
    }
  } else {
    signalWidth = 0;
    signalHeight = 0;
    totalFrames = 0;
    turnSignalAnimation = false;
  }
}

void AnnotatedCameraWidget::initializeFrogPilotWidgets() {
  bottom_layout = new QHBoxLayout();

  distance_btn = new DistanceButton(this);
  bottom_layout->addWidget(distance_btn);

  QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
  bottom_layout->addItem(spacer);

  compass_img = new Compass(this);
  bottom_layout->addWidget(compass_img);

  map_settings_btn_bottom = new MapSettingsButton(this);
  bottom_layout->addWidget(map_settings_btn_bottom);

  main_layout->addLayout(bottom_layout);

  stopSignImg = loadPixmap("../frogpilot/assets/other_images/stop_sign.png", QSize(img_size, img_size));

  animationTimer = new QTimer(this);
  QObject::connect(animationTimer, &QTimer::timeout, [this] {
    animationFrameIndex = (animationFrameIndex + 1) % totalFrames;
  });
}

void AnnotatedCameraWidget::updateFrogPilotVariables(int alert_height, const UIScene &scene) {
  if (is_metric || useSI) {
    accelerationUnit = tr("m/s²");
    leadDistanceUnit = tr(mapOpen ? "m" : "meters");
    leadSpeedUnit = useSI ? tr("m/s") : tr("kph");

    accelerationConversion = 1.0f;
    distanceConversion = 1.0f;
    speedConversion = useSI ? 1.0f : MS_TO_KPH;
  } else {
    accelerationUnit = tr(" ft/s²");
    leadDistanceUnit = tr(mapOpen ? "ft" : "feet");
    leadSpeedUnit = tr("mph");

    accelerationConversion = METER_TO_FOOT;
    distanceConversion = METER_TO_FOOT;
    speedConversion = MS_TO_MPH;
  }

  alertHeight = alert_height;

  alwaysOnLateralActive = scene.always_on_lateral_active;
  showAlwaysOnLateralStatusBar = scene.show_aol_status_bar;

  blindSpotLeft = scene.blind_spot_left;
  blindSpotRight = scene.blind_spot_right;

  cameraView = scene.camera_view;

  compass = scene.compass;
  bool enableCompass = compass && !hideBottomIcons;
  compass_img->setVisible(enableCompass);
  if (enableCompass) {
    compass_img->updateState(scene);
    bottom_layout->setAlignment(compass_img, (rightHandDM ? Qt::AlignLeft : Qt::AlignRight));
  }

  conditionalSpeed = scene.conditional_speed;
  conditionalSpeedLead = scene.conditional_speed_lead;
  conditionalStatus = scene.conditional_status;
  showConditionalExperimentalStatusBar = scene.show_cem_status_bar;

  cruiseAdjustment = scene.disable_curve_speed_smoothing || !is_cruise_set ? fmax(setSpeed - scene.adjusted_cruise, 0) : fmax(0.25 * (setSpeed - scene.adjusted_cruise) + 0.75 * cruiseAdjustment - 1, 0);
  vtscControllingCurve = scene.vtsc_controlling_curve;

  currentAcceleration = scene.acceleration;

  desiredFollow = scene.desired_follow;
  stoppedEquivalence = scene.stopped_equivalence;

  experimentalMode = scene.experimental_mode;

  hideMapIcon = scene.hide_map_icon;
  hideMaxSpeed = scene.hide_max_speed;
  hideSpeed = scene.hide_speed;

  laneDetectionWidth = scene.lane_detection_width;

  leadInfo = scene.lead_info;
  obstacleDistance = scene.obstacle_distance;
  obstacleDistanceStock = scene.obstacle_distance_stock;

  mapOpen = scene.map_open;
  bigMapOpen = mapOpen && scene.big_map;
  map_settings_btn_bottom->setEnabled(map_settings_btn->isEnabled());
  if (map_settings_btn_bottom->isEnabled()) {
    map_settings_btn_bottom->setVisible(!hideBottomIcons && !compass && !hideMapIcon);
    bottom_layout->setAlignment(map_settings_btn_bottom, (rightHandDM ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignBottom);
  }

  modelLength = scene.model_length;

  onroadDistanceButton = scene.onroad_distance_button;
  bool enableDistanceButton = onroadDistanceButton && !hideBottomIcons;
  distance_btn->setVisible(enableDistanceButton);
  if (enableDistanceButton) {
    distance_btn->updateState(scene);
    bottom_layout->setAlignment(distance_btn, (rightHandDM ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignBottom);
  }

  bool enablePedalIcons = scene.pedals_on_ui && !bigMapOpen;
  pedal_icons->setVisible(enablePedalIcons);
  if (enablePedalIcons) {
    pedal_icons->updateState(scene);
  }

  reverseCruise = scene.reverse_cruise;

  roadNameUI = scene.road_name_ui;

  bool enableScreenRecorder = scene.screen_recorder && !mapOpen;
  screenRecorder->setVisible(enableScreenRecorder);
  if (enableScreenRecorder) {
    screenRecorder->updateScreen(scene.fps, scene.started);
  }

  speedLimitController = scene.speed_limit_controller;
  showSLCOffset = speedLimitController && scene.show_slc_offset;
  slcOverridden = speedLimitController && scene.speed_limit_overridden;
  slcSpeedLimitOffset = scene.speed_limit_offset * (is_metric ? MS_TO_KPH : MS_TO_MPH);
  speedLimitChanged = speedLimitController && scene.speed_limit_changed;
  unconfirmedSpeedLimit = speedLimitController ? scene.unconfirmed_speed_limit : 0;
  useViennaSLCSign = scene.use_vienna_slc_sign;

  bool stoppedTimer = scene.stopped_timer && scene.standstill && scene.started_timer / UI_FREQ >= 10 && !mapOpen;
  if (stoppedTimer) {
    if (!standstillTimer.isValid()) {
      standstillTimer.start();
    }
    standstillDuration = standstillTimer.elapsed() / 1000.0;
  } else {
    standstillDuration = 0;
    standstillTimer.invalidate();
  }

  trafficModeActive = scene.traffic_mode_active;

  turnSignalLeft = scene.turn_signal_left;
  turnSignalRight = scene.turn_signal_right;

  useSI = scene.use_si;

  useStockColors = scene.use_stock_colors;
}

void AnnotatedCameraWidget::paintFrogPilotWidgets(QPainter &painter) {
  if ((showAlwaysOnLateralStatusBar || showConditionalExperimentalStatusBar || roadNameUI) && !bigMapOpen) {
    drawStatusBar(painter);
  } else {
    statusBarHeight = 0;
  }

  if (leadInfo && !bigMapOpen) {
    drawLeadInfo(painter);
  }

  if (speedLimitChanged) {
    drawSLCConfirmation(painter);
  }

  if (turnSignalAnimation && (turnSignalLeft || turnSignalRight) && !bigMapOpen && ((!mapOpen && standstillDuration == 0) || signalStyle != "static")) {
    if (!animationTimer->isActive()) {
      animationTimer->start(signalAnimationLength);
    }
    drawTurnSignals(painter);
  } else if (animationTimer->isActive()) {
    animationTimer->stop();
  }
}

Compass::Compass(QWidget *parent) : QWidget(parent) {
  setFixedSize(btn_size * 1.5, btn_size * 1.5);

  compassSize = btn_size;
  circleOffset = compassSize / 2;
  degreeLabelOffset = circleOffset + 25;
  innerCompass = compassSize / 2;

  x = (btn_size * 1.5) / 2 + 20;
  y = (btn_size * 1.5) / 2;

  compassInnerImg = loadPixmap("../frogpilot/assets/other_images/compass_inner.png", QSize(compassSize / 1.75, compassSize / 1.75));
  initializeStaticElements();
}

void Compass::initializeStaticElements() {
  staticElements = QPixmap(size());
  staticElements.fill(Qt::transparent);
  QPainter p(&staticElements);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  p.setPen(QPen(Qt::white, 2));
  p.setBrush(QColor(0, 0, 0, 100));

  const int xOffset = x - circleOffset;
  const int yOffset = y - circleOffset;

  p.drawEllipse(xOffset, yOffset, compassSize, compassSize);
  p.setBrush(Qt::NoBrush);
  const int innerOffset = innerCompass + 5;
  p.drawEllipse(x - innerOffset, y - innerOffset, innerOffset * 2, innerOffset * 2);
  p.drawEllipse(x - degreeLabelOffset, y - degreeLabelOffset, degreeLabelOffset * 2, degreeLabelOffset * 2);

  QPainterPath outerCircle, innerCircle;
  outerCircle.addEllipse(x - degreeLabelOffset, y - degreeLabelOffset, degreeLabelOffset * 2, degreeLabelOffset * 2);
  innerCircle.addEllipse(xOffset, yOffset, compassSize, compassSize);
  p.fillPath(outerCircle.subtracted(innerCircle), Qt::black);
}

void Compass::updateState(const UIScene &scene) {
  if (bearingDeg != scene.bearing_deg) {
    bearingDeg = (scene.bearing_deg + 360) % 360;
    update();
  }
}

void Compass::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  p.drawPixmap(0, 0, staticElements);
  p.translate(x, y);
  p.rotate(bearingDeg);
  p.drawPixmap(-compassInnerImg.width() / 2, -compassInnerImg.height() / 2, compassInnerImg);
  p.resetTransform();

  QFont font = InterFont(10, QFont::Normal);
  const int halfCompassSize = compassSize / 2;
  for (int i = 0; i < 360; i += 15) {
    bool isBold = abs(i - bearingDeg) <= 7;
    font.setWeight(isBold ? QFont::Bold : QFont::Normal);
    p.setFont(font);
    p.setPen(QPen(Qt::white, i % 90 == 0 ? 2 : 1));

    p.save();
    p.translate(x, y);
    p.rotate(i);
    int lineLength = i % 90 == 0 ? 12 : 8;
    p.drawLine(0, -(halfCompassSize - lineLength), 0, -halfCompassSize);
    p.translate(0, -(halfCompassSize + 12));
    p.rotate(-i);
    p.drawText(QRect(-20, -10, 40, 20), Qt::AlignCenter, QString::number(i));
    p.restore();
  }

  p.setFont(InterFont(20, QFont::Bold));
  const std::map<QString, std::tuple<QPair<float, float>, int, QColor>> directionInfo = {
    {"N", {{292.5, 67.5}, Qt::AlignTop | Qt::AlignHCenter, Qt::white}},
    {"E", {{22.5, 157.5}, Qt::AlignRight | Qt::AlignVCenter, Qt::white}},
    {"S", {{112.5, 247.5}, Qt::AlignBottom | Qt::AlignHCenter, Qt::white}},
    {"W", {{202.5, 337.5}, Qt::AlignLeft | Qt::AlignVCenter, Qt::white}}
  };
  const int directionOffset = 20;

  for (const auto &[direction, params] : directionInfo) {
    const auto &[range, alignmentFlag, color] = params;
    const auto &[minRange, maxRange] = range;

    bool isInRange = (minRange > maxRange) ? (bearingDeg >= minRange || bearingDeg <= maxRange) : (bearingDeg >= minRange && bearingDeg <= maxRange);

    QRect textRect(x - innerCompass + directionOffset, y - innerCompass + directionOffset, innerCompass * 2 - 2 * directionOffset, innerCompass * 2 - 2 * directionOffset);

    p.setOpacity(isInRange ? 1.0 : 0.2);
    p.setPen(QPen(color));
    p.drawText(textRect, alignmentFlag, direction);
  }
}

void AnnotatedCameraWidget::drawLeadInfo(QPainter &p) {
  static QElapsedTimer timer;

  static bool isFiveSecondsPassed = false;

  static double maxAcceleration = 0.0;
  constexpr int maxAccelDuration = 5000;

  double acceleration = std::round(currentAcceleration * 100) / 100;

  auto resetTimer = [&]() {
    timer.start();
    isFiveSecondsPassed = false;
  };

  if (acceleration > maxAcceleration && (status == STATUS_ENGAGED || status == STATUS_TRAFFIC_MODE_ACTIVE)) {
    maxAcceleration = acceleration;
    resetTimer();
  } else {
    isFiveSecondsPassed = timer.hasExpired(maxAccelDuration);
  }

  auto createText = [&](const QString &title, double data) {
    return title + QString::number(std::round(data * distanceConversion)) + " " + leadDistanceUnit;
  };

  QString accelText = QString(tr("Accel: %1%2"))
                      .arg(acceleration * accelerationConversion, 0, 'f', 2)
                      .arg(accelerationUnit);

  QString maxAccSuffix;
  if (!mapOpen) {
    maxAccSuffix = QString(tr(" - Max: %1%2"))
                      .arg(maxAcceleration * accelerationConversion, 0, 'f', 2)
                      .arg(accelerationUnit);
  }

  QString obstacleText = createText(mapOpen ? tr(" | Obstacle: ") : tr("  |  Obstacle Factor: "), obstacleDistance);
  QString stopText = createText(mapOpen ? tr(" - Stop: ") : tr("  -  Stop Factor: "), stoppedEquivalence);
  QString followText = " = " + createText(mapOpen ? tr("Follow: ") : tr("Follow Distance: "), desiredFollow);

  auto createDiffText = [&](double data, double stockData) {
    double difference = std::round((data - stockData) * distanceConversion);
    return difference != 0 ? QString(" (%1%2)").arg(difference > 0 ? "+" : "").arg(difference) : QString();
  };

  p.save();

  QRect insightsRect(rect().left() - 1, rect().top() - 60, rect().width() + 2, 100);
  p.setBrush(QColor(0, 0, 0, 150));
  p.drawRoundedRect(insightsRect, 30, 30);
  p.setFont(InterFont(28, QFont::Bold));
  p.setRenderHint(QPainter::TextAntialiasing);

  QRect adjustedRect = insightsRect.adjusted(0, 27, 0, 27);
  int textBaseLine = adjustedRect.y() + (adjustedRect.height() + p.fontMetrics().height()) / 2 - p.fontMetrics().descent();

  QStringList texts = {accelText, maxAccSuffix, obstacleText, createDiffText(obstacleDistance, obstacleDistanceStock), stopText, followText};
  QList<QColor> colors = {Qt::white, isFiveSecondsPassed ? Qt::white : redColor(), Qt::white, (obstacleDistance - obstacleDistanceStock) > 0 ? Qt::green : Qt::red, Qt::white, Qt::white};

  int totalTextWidth = 0;
  for (const auto &text : texts) {
    totalTextWidth += p.fontMetrics().horizontalAdvance(text);
  }

  int textStartPos = adjustedRect.x() + (adjustedRect.width() - totalTextWidth) / 2;

  for (int i = 0; i < texts.size(); ++i) {
    p.setPen(colors[i]);
    p.drawText(textStartPos, textBaseLine, texts[i]);
    textStartPos += p.fontMetrics().horizontalAdvance(texts[i]);
  }

  p.restore();
}

PedalIcons::PedalIcons(QWidget *parent) : QWidget(parent) {
  setFixedSize(btn_size, btn_size);

  brake_pedal_img = loadPixmap("../frogpilot/assets/other_images/brake_pedal.png", QSize(img_size, img_size));
  gas_pedal_img = loadPixmap("../frogpilot/assets/other_images/gas_pedal.png", QSize(img_size, img_size));
}

void PedalIcons::updateState(const UIScene &scene) {
  acceleration = scene.acceleration;
  brakeLightOn = scene.brake_lights_on;
  dynamicPedals = scene.dynamic_pedals_on_ui;
  standstill = scene.standstill;
  staticPedals = scene.static_pedals_on_ui;

  accelerating = acceleration > 0.25f;
  decelerating = acceleration < -0.25f;

  if (accelerating || decelerating) {
    update();
  }
}

void PedalIcons::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  int totalWidth = 2 * img_size;
  int startX = (width() - totalWidth) / 2;

  int brakeX = startX + img_size / 2;
  int gasX = startX + img_size;

  float brakeOpacity = 1.0f;
  float gasOpacity = 1.0f;

  if (dynamicPedals) {
    brakeOpacity = standstill ? 1.0f : decelerating ? std::max(0.25f, std::abs(acceleration)) : 0.25f;
    gasOpacity = accelerating ? std::max(0.25f, acceleration) : 0.25f;
  } else if (staticPedals) {
    brakeOpacity = standstill || brakeLightOn || acceleration < -0.5f ? 1.0f : 0.25f;
    gasOpacity = !standstill && acceleration > 0 ? 1.0f : 0.25f;
  }

  p.setOpacity(brakeOpacity);
  p.drawPixmap(brakeX, (height() - img_size) / 2, brake_pedal_img);

  p.setOpacity(gasOpacity);
  p.drawPixmap(gasX, (height() - img_size) / 2, gas_pedal_img);
}

void AnnotatedCameraWidget::drawSLCConfirmation(QPainter &p) {
  p.save();

  QSize size = this->size();
  int halfWidth = size.width() / 2;

  QRect leftRect(0, 0, halfWidth, size.height());
  QRect rightRect(halfWidth, 0, halfWidth, size.height());

  p.setOpacity(0.5);
  p.fillRect(leftRect, rightHandDM ? redColor() : greenColor());
  p.fillRect(rightRect, rightHandDM ? greenColor() : redColor());
  p.setOpacity(1.0);

  p.setFont(InterFont(75, QFont::Bold));
  p.setPen(Qt::white);

  QString unitText = is_metric ? tr("kph") : tr("mph");
  QString speedText = QString::number(std::nearbyint(unconfirmedSpeedLimit * (is_metric ? MS_TO_KPH : MS_TO_MPH))) + " " + unitText;
  QString confirmText = tr("Confirm speed limit\n") + speedText;
  QString ignoreText = tr("Ignore speed limit\n") + speedText;

  QRect textRect(0, leftRect.height() / 2 - 225, halfWidth, leftRect.height() / 2);

  p.drawText(textRect.translated(0, 0), Qt::AlignCenter, rightHandDM ? ignoreText : confirmText);
  p.drawText(textRect.translated(halfWidth, 0), Qt::AlignCenter, rightHandDM ? confirmText : ignoreText);

  p.restore();
}

void AnnotatedCameraWidget::drawStatusBar(QPainter &p) {
  p.save();

  static QElapsedTimer timer;
  static QString lastShownStatus;

  static bool displayStatusText = false;

  constexpr qreal fadeDuration = 1500.0;
  constexpr qreal textDuration = 5000.0;

  static qreal roadNameOpacity = 0.0;
  static qreal statusTextOpacity = 0.0;

  QString newStatus;

  int offset = 50;
  QRect statusBarRect(rect().left() - 1, rect().bottom() - offset, rect().width() + 2, 100);
  statusBarHeight = statusBarRect.height() - offset;
  p.setBrush(QColor(0, 0, 0, 150));
  p.setOpacity(1.0);
  p.drawRoundedRect(statusBarRect, 30, 30);

  int modelStopTime = std::nearbyint(modelLength / (speed / (is_metric ? MS_TO_KPH : MS_TO_MPH)));

  std::map<int, QString> conditionalStatusMap = {
    {0, tr("Conditional Experimental Mode ready")},
    {1, tr("Conditional Experimental overridden")},
    {2, tr("Experimental Mode manually activated")},
    {3, tr("Conditional Experimental overridden")},
    {4, tr("Experimental Mode manually activated")},
    {5, tr("Conditional Experimental overridden")},
    {6, tr("Experimental Mode manually activated")},
    {7, tr("Experimental Mode activated for %1").arg(mapOpen ? tr("low speed") : tr("speed being less than %1 %2").arg(conditionalSpeedLead).arg(is_metric ? tr("kph") : tr("mph")))},
    {8, tr("Experimental Mode activated for %1").arg(mapOpen ? tr("low speed") : tr("speed being less than %1 %2").arg(conditionalSpeed).arg(is_metric ? tr("kph") : tr("mph")))},
    {9, tr("Experimental Mode activated for turn") + (mapOpen ? " signal" : tr(" / lane change"))},
    {10, tr("Experimental Mode activated for intersection")},
    {11, tr("Experimental Mode activated for upcoming turn")},
    {12, tr("Experimental Mode activated for curve")},
    {13, tr("Experimental Mode activated for stopped lead")},
    {14, tr("Experimental Mode activated for slower lead")},
    {15, tr("Experimental Mode activated %1").arg(mapOpen || modelStopTime < 1 || speed < 1 ? tr("to stop") : QString("for the model wanting to stop in %1 seconds").arg(modelStopTime))},
    {16, tr("Experimental Mode forced on %1").arg(mapOpen || modelStopTime < 1 || speed < 1 ? tr("to stop") : QString("for the model wanting to stop in %1 seconds").arg(modelStopTime))},
    {17, tr("Experimental Mode activated due to no speed limit")},
  };

  if (alwaysOnLateralActive && showAlwaysOnLateralStatusBar) {
    newStatus = tr("Always On Lateral active") + (mapOpen ? "" : tr(". Press the \"Cruise Control\" button to disable"));
  } else if (showConditionalExperimentalStatusBar) {
    newStatus = conditionalStatusMap.at(conditionalStatus);
  }

  static const std::map<int, QString> suffixMap = {
    {1, tr(". Long press the \"distance\" button to revert")},
    {2, tr(". Long press the \"distance\" button to revert")},
    {3, tr(". Click the \"LKAS\" button to revert")},
    {4, tr(". Click the \"LKAS\" button to revert")},
    {5, tr(". Double tap the screen to revert")},
    {6, tr(". Double tap the screen to revert")},
  };

  if (!alwaysOnLateralActive && !mapOpen && !newStatus.isEmpty()) {
    if (suffixMap.find(conditionalStatus) != suffixMap.end()) {
      newStatus += suffixMap.at(conditionalStatus);
    }
  }

  QString roadName = QString::fromStdString(paramsMemory.get("RoadName"));
  roadName = (!roadNameUI || roadName.isEmpty() || roadName == "null") ? "" : roadName;

  if (newStatus != lastShownStatus || roadName.isEmpty()) {
    lastShownStatus = newStatus;
    displayStatusText = true;
    timer.restart();
  } else if (displayStatusText && timer.hasExpired(textDuration + fadeDuration)) {
    displayStatusText = false;
  }

  if (displayStatusText) {
    statusTextOpacity = qBound(0.0, 1.0 - (timer.elapsed() - textDuration) / fadeDuration, 1.0);
    roadNameOpacity = 1.0 - statusTextOpacity;
  } else {
    roadNameOpacity = qBound(0.0, timer.elapsed() / fadeDuration, 1.0);
    statusTextOpacity = 0.0;
  }

  p.setFont(InterFont(40, QFont::Bold));
  p.setOpacity(statusTextOpacity);
  p.setPen(Qt::white);
  p.setRenderHint(QPainter::TextAntialiasing);

  QRect textRect = p.fontMetrics().boundingRect(statusBarRect, Qt::AlignCenter | Qt::TextWordWrap, newStatus);
  textRect.moveBottom(statusBarRect.bottom() - offset);
  p.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, newStatus);

  if (!roadName.isEmpty()) {
    p.setOpacity(roadNameOpacity);
    textRect = p.fontMetrics().boundingRect(statusBarRect, Qt::AlignCenter | Qt::TextWordWrap, roadName);
    textRect.moveBottom(statusBarRect.bottom() - offset);
    p.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, roadName);
  }

  p.restore();
}

void AnnotatedCameraWidget::drawTurnSignals(QPainter &p) {
  p.setRenderHint(QPainter::Antialiasing);

  bool blindspotActive = turnSignalLeft ? blindSpotLeft : blindSpotRight;

  if (signalStyle == "static") {
    int signalXPosition = turnSignalLeft ? (rect().center().x() * 0.75) - signalWidth : rect().center().x() * 1.25;
    int signalYPosition = signalHeight / 2;

    if (blindspotActive && !blindspotImages.empty()) {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, blindspotImages[turnSignalLeft ? 0 : 1]);
    } else {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, signalImages[2 * animationFrameIndex + (turnSignalLeft ? 0 : 1)]);
    }
  } else if (signalStyle == "traditional") {
    int signalXPosition = turnSignalLeft ? width() - ((animationFrameIndex + 1) * signalWidth) : animationFrameIndex * signalWidth;
    int signalYPosition = height() - signalHeight;

    signalYPosition -= fmax(alertHeight, statusBarHeight);

    if (blindspotActive && !blindspotImages.empty()) {
      p.drawPixmap(turnSignalLeft ? width() - signalWidth : 0, signalYPosition, signalWidth, signalHeight, blindspotImages[turnSignalLeft ? 0 : 1]);
    } else {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, signalImages[2 * animationFrameIndex + (turnSignalLeft ? 0 : 1)]);
    }
  } else if (signalStyle == "traditional_gif") {
    int signalXPosition = turnSignalLeft ? width() - (animationFrameIndex * signalMovement) + signalWidth : (animationFrameIndex * signalMovement) - signalWidth;
    int signalYPosition = height() - signalHeight;

    signalYPosition -= fmax(alertHeight, statusBarHeight);

    if (blindspotActive && !blindspotImages.empty()) {
      p.drawPixmap(turnSignalLeft ? width() - signalWidth : 0, signalYPosition, signalWidth, signalHeight, blindspotImages[turnSignalLeft ? 0 : 1]);
    } else {
      p.drawPixmap(signalXPosition, signalYPosition, signalWidth, signalHeight, signalImages[2 * animationFrameIndex + (turnSignalLeft ? 0 : 1)]);
    }
  }
}
