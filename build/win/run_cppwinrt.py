import sys
import subprocess

def main():
    cppwinrt_exe = sys.argv[1]
    args = sys.argv[2:]

    # Run cppwinrt.exe with provided arguments
    print(f"============== RAFAL: Running cppwinrt.exe [{cppwinrt_exe}] with args: {args}")
    subprocess.check_call([cppwinrt_exe] + args)

if __name__ == '__main__':
    sys.exit(main())
