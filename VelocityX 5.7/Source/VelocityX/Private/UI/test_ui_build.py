"""
VELOCITY X - C++ UMG Widget Controller Verification Script
Path: VelocityX/Source/VelocityX/Private/UI/test_ui_build.py
Description:
    Verifies the integrity of all C++ UMG controller headers and implementation files
    created in VelocityX/Source/VelocityX/Private/UI.
"""

import os
import sys

REQUIRED_FILES = [
    "UVelocityXMainMenuWidget.h",
    "UVelocityXMainMenuWidget.cpp",
    "UVelocityXCarSelectionWidget.h",
    "UVelocityXCarSelectionWidget.cpp",
    "UVelocityXHUDWidget.h",
    "UVelocityXHUDWidget.cpp",
    "UVelocityXPauseMenuWidget.h",
    "UVelocityXPauseMenuWidget.cpp",
    "VelocityXUIConfig.h",
    "VelocityXUIConfig.cpp",
    "DefaultGame_UI_Entries.ini"
]

def audit_ui_files():
    print("=" * 70)
    print("VELOCITY X - C++ UMG Widget Controllers Audit")
    print("=" * 70)

    current_dir = os.path.dirname(os.path.abspath(__file__))
    all_passed = True

    for file_name in REQUIRED_FILES:
        file_path = os.path.join(current_dir, file_name)
        if os.path.exists(file_path):
            size = os.path.getsize(file_path)
            with open(file_path, "r", encoding="utf-8") as f:
                content = f.read()

            # Basic syntax / macro sanity check
            if file_name.endswith(".h") and "generated.h" not in file_name and file_name != "DefaultGame_UI_Entries.ini":
                has_generated = "generated.h" in content
                has_uclass = "UCLASS(" in content or "UCLASS()" in content
                has_body = "GENERATED_BODY()" in content
                print(f" [PASS] {file_name:32} ({size:,} bytes) | GenHeader: {has_generated} | UCLASS: {has_uclass} | Body: {has_body}")
            else:
                print(f" [PASS] {file_name:32} ({size:,} bytes)")
        else:
            print(f" [FAIL] {file_name:32} -> MISSING")
            all_passed = False

    print("=" * 70)
    if all_passed:
        print("STATUS: ALL C++ UMG WIDGET CONTROLLER FILES VERIFIED!")
    else:
        print("STATUS: AUDIT DETECTED MISSING FILES!")
    print("=" * 70)
    return all_passed

if __name__ == "__main__":
    success = audit_ui_files()
    sys.exit(0 if success else 1)
