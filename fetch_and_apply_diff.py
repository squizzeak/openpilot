#!/usr/bin/env python3
"""
Fetch and apply the brake hold commit from j-vanetten's repository
"""
import urllib.request
import subprocess
import sys
import os

def fetch_diff():
    """Fetch the diff from GitHub"""
    url = "https://github.com/j-vanetten/openpilot/commit/f75abf2363c983c8d49ed411634254756c17f797.patch"

    print(f"Fetching patch from: {url}")
    try:
        with urllib.request.urlopen(url) as response:
            diff_content = response.read().decode('utf-8')
            return diff_content
    except Exception as e:
        print(f"Error fetching patch: {e}", file=sys.stderr)
        return None

def save_diff(diff_content):
    """Save diff to file"""
    filename = "brake-hold.patch"
    with open(filename, 'w') as f:
        f.write(diff_content)
    print(f"Patch saved to {filename}")
    return filename

def apply_patch(filename):
    """Apply the patch using git apply"""
    print("\n🔍 Checking if patch can be applied cleanly...")

    # First check if patch applies cleanly
    check_result = subprocess.run(
        ['git', 'apply', '--check', filename],
        capture_output=True,
        text=True
    )

    if check_result.returncode == 0:
        print("✅ Patch can be applied cleanly!")

        # Apply the patch
        print("🔧 Applying patch...")
        apply_result = subprocess.run(
            ['git', 'apply', filename],
            capture_output=True,
            text=True
        )

        if apply_result.returncode == 0:
            print("✨ Patch applied successfully!")
            return True
        else:
            print(f"❌ Failed to apply patch: {apply_result.stderr}")
            return False
    else:
        print("⚠️  Patch does not apply cleanly. Trying with 3-way merge...")

        # Try with 3-way merge
        apply_result = subprocess.run(
            ['git', 'apply', '--3way', filename],
            capture_output=True,
            text=True
        )

        if apply_result.returncode == 0:
            print("✨ Patch applied with 3-way merge!")
            return True
        else:
            print(f"❌ Failed to apply patch even with 3-way merge: {apply_result.stderr}")
            print("\n💡 Tip: You may need to resolve conflicts manually or apply changes by hand.")
            return False

def show_patch_summary(diff_content):
    """Show a summary of the patch"""
    lines = diff_content.split('\n')

    # Extract commit info
    print("\n📋 Commit Information:")
    for line in lines[:20]:
        if line.startswith('From:') or line.startswith('Date:') or line.startswith('Subject:'):
            print(f"  {line}")

    # Count files changed
    files_changed = set()
    additions = 0
    deletions = 0

    for line in lines:
        if line.startswith('+++') or line.startswith('---'):
            if not line.startswith('+++') and not line.startswith('--- /dev/null'):
                file_path = line[6:] if line.startswith('--- a/') else line[6:]
                if file_path and file_path != '/dev/null':
                    files_changed.add(file_path)
        elif line.startswith('+') and not line.startswith('+++'):
            additions += 1
        elif line.startswith('-') and not line.startswith('---'):
            deletions += 1

    print(f"\n📊 Statistics:")
    print(f"  Files changed: {len(files_changed)}")
    print(f"  Lines added: {additions}")
    print(f"  Lines removed: {deletions}")

    if files_changed:
        print("\n📁 Files affected:")
        for f in sorted(files_changed):
            print(f"  • {f}")

def main():
    """Main function"""
    # Fetch the diff
    diff_content = fetch_diff()
    if not diff_content:
        sys.exit(1)

    # Show patch summary
    show_patch_summary(diff_content)

    # Save to file
    filename = save_diff(diff_content)

    # Also save as text file for easier analysis
    with open("brake-hold-analysis.txt", 'w') as f:
        f.write(diff_content)
    print(f"📄 Analysis file saved to 'brake-hold-analysis.txt'")

    print("\n" + "="*60)
    print("The patch has been fetched and saved.")
    print("Due to codebase differences, manual analysis and porting is recommended.")
    print(f"\n📄 Full patch saved to '{filename}'")
    print("📝 Text version saved to 'brake-hold-analysis.txt'")
    print("\n💡 Next steps:")
    print("  1. Review the changes in brake-hold-analysis.txt")
    print("  2. Find corresponding files in the local codebase")
    print("  3. Manually apply relevant changes")
    print("="*60)

    print("="*60)

if __name__ == "__main__":
    main()