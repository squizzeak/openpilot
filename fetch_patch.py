#!/usr/bin/env python3
"""Fetch the brake hold patch from GitHub"""
import urllib.request
import json

url = "https://api.github.com/repos/j-vanetten/openpilot/commits/f75abf2363c983c8d49ed411634254756c17f797"
patch_url = "https://github.com/j-vanetten/openpilot/commit/f75abf2363c983c8d49ed411634254756c17f797.patch"

# Fetch commit info
try:
    with urllib.request.urlopen(url) as response:
        data = json.loads(response.read().decode())
        print("Commit Information:")
        print(f"  Author: {data['commit']['author']['name']}")
        print(f"  Date: {data['commit']['author']['date']}")
        print(f"  Message: {data['commit']['message']}")
        print(f"  Files changed: {data['stats']['total']}")
        print(f"  Additions: {data['stats']['additions']}")
        print(f"  Deletions: {data['stats']['deletions']}")

        print("\nFiles modified:")
        for file in data['files']:
            print(f"  - {file['filename']} (+{file['additions']}, -{file['deletions']})")
            if file.get('patch'):
                # Save individual patches
                with open(f"{file['filename'].replace('/', '_')}.patch", 'w') as f:
                    f.write(file['patch'])
except:
    pass

# Fetch full patch
try:
    with urllib.request.urlopen(patch_url) as response:
        patch = response.read().decode()
        with open("brake-hold-full.patch", 'w') as f:
            f.write(patch)
        print("\nFull patch saved to brake-hold-full.patch")
except Exception as e:
    print(f"Error: {e}")