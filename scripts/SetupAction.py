import os
import subprocess
import platform

os.chdir('./../') # Change from devtools/scripts directory to root

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if platform.system() == "Windows":
    print("\nRunning premake...")
    subprocess.call([os.path.abspath("./scripts/Win-GenProjects.bat"), "nopause"])
    print("\nSetup completed!")
else:
    print("StarEngine requires Windows to generate project files.")
