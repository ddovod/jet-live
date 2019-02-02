#! /usr/bin/env python

import argparse
import subprocess
import os


sourceDir = None
buildDir = None
binDir = None


def processCommand(cmdStr):
    if sourceDir is None:
        raise RuntimeError("Source directory is not set")

    enableTags = []
    disableTags = []
    cmdStr = cmdStr[9:]
    cmds = cmdStr.split(";")
    for el in cmds:
        cmd = el.strip()
        if cmd.startswith("enable"):
            for tag in cmd[7:-1].split(","):
                enableTags.append(tag)
        elif cmd.startswith("disable"):
            for tag in cmd[8:-1].split(","):
                disableTags.append(tag)
        else:
            raise RuntimeError("Unknown command")

    cmakeFileWasModified = False
    for root, subdirs, files in os.walk(sourceDir):
        for filename in files:
            filepath = os.path.join(root, filename)
            isCmakeLists = filepath.endswith("CMakeLists.txt")
            commentStr = "//"
            if isCmakeLists:
                commentStr = "#"
            lines = open(filepath, "r").readlines()
            newFileLines = []
            fileWasModified = False
            for line in lines:
                uncommentedLine = line
                if line.startswith(commentStr):
                    uncommentedLine = line[len(commentStr):]
                strippedLine = uncommentedLine.strip()
                tagsPos = strippedLine.find("<jet_tag:")
                if tagsPos == -1:
                    newFileLines.append(line)
                    continue
                tagsStr = strippedLine[tagsPos + 9:-1].split(",")
                tags = []
                for el in tagsStr:
                    tags.append(el.strip())
                if len([v for v in tags if v in disableTags]) > 0:
                    newFileLines.append(commentStr + uncommentedLine)
                    fileWasModified = True
                    if isCmakeLists:
                        cmakeFileWasModified = True
                elif len([v for v in tags if v in enableTags]):
                    fileWasModified = True
                    if isCmakeLists:
                        cmakeFileWasModified = True
                    newFileLines.append(uncommentedLine)
                else:
                    newFileLines.append(line)
            if fileWasModified:
                print("RUNNER: Patching source file: " + filepath)
                with open(filepath, "w") as f:
                    for line in newFileLines:
                        f.write(line)

    if cmakeFileWasModified:
        print("RUNNER: Running cmake")
        cmd = "cmake -DCMAKE_BUILD_TYPE=Debug -DJET_LIVE_BUILD_TESTS=ON .."
        subprocess.Popen(cmd.split(" "), cwd=buildDir).wait()


parser = argparse.ArgumentParser()
parser.add_argument('-b', '--build_directory',
                    help="Path to the build directory",
                    required=True)
parser.add_argument('-d', '--binary_directory',
                    help="Path to the binary directory",
                    required=True)
parser.add_argument('-s', '--source_directory',
                    help="Path to the source directory",
                    required=True)
args = parser.parse_args()
sourceDir = os.path.realpath(os.path.expanduser(args.source_directory))
buildDir = os.path.realpath(os.path.expanduser(args.build_directory))

testCmd = [os.path.join(args.binary_directory, "tests"),
           "--use-colour=yes"]
print("RUNNER: Running '" + " ".join(testCmd) + "'")
proc = subprocess.Popen(testCmd, shell=True,
                        stdout=subprocess.PIPE)

while proc.poll() is None:
    output = proc.stdout.readline()
    if output:
        output = output.strip()
        if output.startswith('JET_TEST: '):
            processCommand(output)
        else:
            print(output)

exit(proc.poll())
