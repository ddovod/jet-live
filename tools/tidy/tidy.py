#!/usr/bin/env python

import subprocess
import json
import os
import argparse


def getDefaultCompilerIncludes():
    compPathsCmd = "c++ -E -x c++ - -v < /dev/null"
    proc = subprocess.Popen(compPathsCmd, shell=True,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    _, err = proc.communicate()
    includePaths = []
    includesStarted = False
    for line in err.splitlines():
        if line.startswith('#include <...> search starts here'):
            includesStarted = True
        elif line.startswith('End of search list.'):
            break
        elif includesStarted:
            includePaths.append(line)
    return includePaths


def getScriptDir():
    return os.path.dirname(os.path.realpath(__file__))


def fileIsInSubdirectories(filepath, directoriesList):
    realFilepath = os.path.realpath(os.path.expanduser(filepath))
    for el in directoriesList:
        fullPath = os.path.join(os.getcwd(), el)
        realDirPath = os.path.realpath(os.path.expanduser(fullPath))
        if realFilepath.startswith(realDirPath):
            return True
    return False


parser = argparse.ArgumentParser()
parser.add_argument('-c', '--compile_commands_path',
                    help="Path to the 'compile_commands.json' file")
parser.add_argument('-d', '--source_directories',
                    help="Paths to the directories with sources to check, " +
                    "only files from these directories will be checked",
                    nargs='+', type=str)
args = parser.parse_args()

compileCommandsPath = args.compile_commands_path
if compileCommandsPath is None or not os.path.exists(compileCommandsPath):
    parser.print_help()
    exit(0)

directoriesToCheck = args.source_directories

defaultCompilerIncludes = getDefaultCompilerIncludes()
compileCommands = json.load(open(compileCommandsPath))
newCompileCommands = []
for cu in compileCommands:
    if not fileIsInSubdirectories(cu['file'], directoriesToCheck):
        continue
    command = cu['command']
    firstWhitespaceIndex = command.find(' ')
    if firstWhitespaceIndex == -1:
        raise RuntimeError("wtf?")
    newCu = cu
    newCu['command'] = command[:firstWhitespaceIndex] + ' '
    for el in defaultCompilerIncludes:
        newCu['command'] = newCu['command'] + ' -isystem ' + el + ' '
    newCu['command'] = newCu['command'] + command[firstWhitespaceIndex:]
    newCompileCommands.append(newCu)

tidyOutDir = os.path.join(os.getcwd(), 'tidy_report')
if not os.path.exists(tidyOutDir):
    os.makedirs(tidyOutDir)
compileCommandsPath = os.path.join(tidyOutDir, 'compile_commands.json')
json.dump(newCompileCommands, open(compileCommandsPath, 'w'))

tidyConfig = open('.clang-tidy', 'r').read()

runClangTidyScriptPath = os.path.join(getScriptDir(), 'run-clang-tidy.py ')
subprocess.Popen(runClangTidyScriptPath +
                 '-fix ' +
                 '-export-fixes ./tidy_report/fixes.yaml -j 4 ' +
                 '-header-filter "' + os.getcwd() + '/libs/**" ' +
                 '-quiet -p ./tidy_report',
                 shell=True).wait()
