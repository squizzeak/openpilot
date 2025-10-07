#!/usr/bin/env python3
"""
Script to help port brake hold changes from j-vanetten's commit to local codebase
"""
import os
import sys
import re

def analyze_local_codebase():
    """Analyze local codebase for brake hold implementation points"""

    print("="*60)
    print("LOCAL CODEBASE ANALYSIS")
    print("="*60)

    # Files where brake hold is already referenced
    existing_files = {
        "events.py": "selfdrive/controls/lib/events.py",
        "interfaces.py": "selfdrive/car/interfaces.py",
        "honda/carstate.py": "selfdrive/car/honda/carstate.py",
        "hyundai/carstate.py": "selfdrive/car/hyundai/carstate.py",
        "toyota/carstate.py": "selfdrive/car/toyota/carstate.py"
    }

    print("\n📋 Existing brake hold references:")
    for name, path in existing_files.items():
        if os.path.exists(path):
            print(f"  ✅ {name}: {path}")
        else:
            print(f"  ❌ {name}: {path} (not found)")

    # Key files for brake hold implementation
    implementation_files = {
        "controlsd.py": "selfdrive/controls/controlsd.py",
        "longitudinal_planner.py": "selfdrive/controls/lib/longitudinal_planner.py",
        "params.py": "common/params.py",
        "car.capnp": "cereal/car.capnp"
    }

    print("\n📁 Key implementation files:")
    for name, path in implementation_files.items():
        if os.path.exists(path):
            print(f"  ✅ {name}: {path}")
        else:
            print(f"  ❌ {name}: {path} (not found)")

    return existing_files, implementation_files

def show_porting_instructions():
    """Display instructions for manual porting"""

    print("\n" + "="*60)
    print("MANUAL PORTING INSTRUCTIONS")
    print("="*60)

    print("""
1. FETCH THE PATCH:
   Run one of these commands:
   $ curl -o brake-hold-orig.patch "https://github.com/j-vanetten/openpilot/commit/f75abf2363c983c8d49ed411634254756c17f797.patch"
   $ wget -O brake-hold-orig.patch "https://github.com/j-vanetten/openpilot/commit/f75abf2363c983c8d49ed411634254756c17f797.patch"

2. ANALYZE THE PATCH:
   $ python3 port_brake_hold.py analyze brake-hold-orig.patch

3. PORT THE CHANGES:
   The script will identify which changes need to be ported and where.
""")

def analyze_patch(patch_file):
    """Analyze a patch file and map to local codebase"""

    if not os.path.exists(patch_file):
        print(f"❌ Patch file not found: {patch_file}")
        print("Please fetch the patch first using the instructions above.")
        return

    print("="*60)
    print("PATCH ANALYSIS")
    print("="*60)

    with open(patch_file, 'r') as f:
        content = f.read()

    # Extract file changes
    files_changed = []
    current_file = None
    changes = {}

    for line in content.split('\n'):
        if line.startswith('diff --git'):
            match = re.search(r'b/(.+?)$', line)
            if match:
                current_file = match.group(1)
                files_changed.append(current_file)
                changes[current_file] = {'additions': [], 'deletions': []}
        elif current_file:
            if line.startswith('+') and not line.startswith('+++'):
                changes[current_file]['additions'].append(line[1:])
            elif line.startswith('-') and not line.startswith('---'):
                changes[current_file]['deletions'].append(line[1:])

    print(f"\n📊 Files modified: {len(files_changed)}")
    for file in files_changed:
        add_count = len(changes[file]['additions'])
        del_count = len(changes[file]['deletions'])
        print(f"  • {file} (+{add_count}, -{del_count})")

    # Map to local structure
    print("\n🗺️ Mapping to local codebase:")

    mapping = {
        'controlsd.py': 'selfdrive/controls/controlsd.py',
        'longitudinal_planner.py': 'selfdrive/controls/lib/longitudinal_planner.py',
        'carstate.py': 'selfdrive/car/*/carstate.py',
        'carcontroller.py': 'selfdrive/car/*/carcontroller.py',
        'interface.py': 'selfdrive/car/*/interface.py',
        'params.py': 'common/params.py',
        'events.py': 'selfdrive/controls/lib/events.py'
    }

    for changed_file in files_changed:
        filename = os.path.basename(changed_file)
        if filename in mapping:
            local_path = mapping[filename]
            if '*' in local_path:
                print(f"  📍 {changed_file} -> {local_path} (check all car brands)")
            else:
                if os.path.exists(local_path):
                    print(f"  ✅ {changed_file} -> {local_path}")
                else:
                    print(f"  ⚠️  {changed_file} -> {local_path} (not found locally)")
        else:
            print(f"  ❓ {changed_file} -> Need to find local equivalent")

    # Extract brake hold specific changes
    print("\n🔧 Brake Hold Specific Changes:")

    brake_hold_patterns = [
        r'brake.*hold',
        r'brakeHold',
        r'BRAKE_HOLD',
        r'brake_hold',
        r'BrakeHold'
    ]

    for file, file_changes in changes.items():
        brake_hold_adds = []
        brake_hold_dels = []

        for line in file_changes['additions']:
            for pattern in brake_hold_patterns:
                if re.search(pattern, line, re.IGNORECASE):
                    brake_hold_adds.append(line.strip())
                    break

        for line in file_changes['deletions']:
            for pattern in brake_hold_patterns:
                if re.search(pattern, line, re.IGNORECASE):
                    brake_hold_dels.append(line.strip())
                    break

        if brake_hold_adds or brake_hold_dels:
            print(f"\n  📄 {file}:")
            if brake_hold_adds:
                print("    Additions:")
                for line in brake_hold_adds[:5]:  # Show first 5
                    print(f"      + {line[:80]}")
            if brake_hold_dels:
                print("    Deletions:")
                for line in brake_hold_dels[:5]:  # Show first 5
                    print(f"      - {line[:80]}")

    # Generate porting checklist
    print("\n" + "="*60)
    print("PORTING CHECKLIST")
    print("="*60)

    print("""
Based on the patch analysis, here's what needs to be done:

1. [ ] Review brake hold state management in controlsd.py
2. [ ] Check longitudinal planner modifications
3. [ ] Update car-specific carstate.py files for brake hold detection
4. [ ] Add brake hold parameters to params.py if needed
5. [ ] Update events.py with any new brake hold events
6. [ ] Test brake hold activation/deactivation logic
7. [ ] Verify safety checks are in place
8. [ ] Add UI elements for brake hold status (if applicable)

⚠️ IMPORTANT: Since the codebases differ, you'll need to:
   - Adapt the logic to fit your existing structure
   - Ensure compatibility with FrogPilot features
   - Test thoroughly before using on a vehicle
""")

def main():
    """Main function"""

    if len(sys.argv) > 1:
        if sys.argv[1] == 'analyze' and len(sys.argv) > 2:
            # Analyze a patch file
            analyze_patch(sys.argv[2])
        else:
            print("Usage: python3 port_brake_hold.py [analyze <patch-file>]")
    else:
        # Default: analyze local codebase and show instructions
        analyze_local_codebase()
        show_porting_instructions()

        print("\n💡 Once you have the patch, run:")
        print("   python3 port_brake_hold.py analyze brake-hold-orig.patch")

if __name__ == "__main__":
    main()