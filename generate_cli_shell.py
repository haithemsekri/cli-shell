import os
import time
import sys
import xml.etree.ElementTree as ET
import pycksum
from collections import namedtuple

CliCmdType = namedtuple('CliCmdType', ['C', 'P', 'A'])

cli_cmds_list = []

def printCli(root, path):
    """Recursively prints the tree."""
    path = path + root.attrib.get('name', root.text).format().strip('\n').strip(' ')
    if path:
      path += '_'
    for elem in root.getchildren():
      if elem.tag.title() == "Class":
        printCli(elem, path)
      elif elem.tag.title() == "Method":
        argList = ""
        for arg in elem.getchildren():
          if arg.tag.title() == "Argument":
            argList += arg.attrib.get('type', arg.text) + ":" + arg.attrib.get('name', arg.text) + " "

        fullPath = path + elem.attrib.get('name', elem.text)
        ck = pycksum.cksum(fullPath)
        cli_cmds_list.append(CliCmdType(C=ck, P=fullPath, A=argList))
        print '0x%08x %s %s' % \
          (ck, fullPath, argList)

tree = ET.parse('cli_cmd.xml')
root = tree.getroot()
path = ""
printCli(root, path)

#Sort list by id
cli_cmds_list = sorted(cli_cmds_list, key=lambda CliCmdType: CliCmdType[0])

#Load cli_shell_head.py.inc
f = open("cli_shell_head.py.inc", "r")
header = f.read()
f.close()

#Load cli_shell_tail.py.inc
f = open("cli_shell_tail.py.inc", "r")
tail = f.read()
f.close()

#bake header, commands and tail in one file
f = open("cli_shell.py","w")
f.write(header)

def AppendCmdToFile(id, cmd, args):
  cmd = cmd.strip('\n').strip(' ')
  args = args.strip('\n').strip(' ')
  buf =  """\n    def do_%s(self, inp):\n      CliSendIp(0x%08x, "%s", "%s", "{}".format(inp))\n""" % (cmd, id, cmd, args)
  f.write(buf)

for x in cli_cmds_list:
  AppendCmdToFile(x.C, x.P, x.A)

f.write(tail)
f.close()

# Create C header file.
f = open("cli_shell.h","w")

f.write("""\n
/**********************************************************************/
/****************** Genrated File avoid to modify *********************/
/**********************************************************************/

#include <stdint.h>
#include <string.h>


typedef struct {
  const uint32_t u32Id;
  int32_t (*pftCliCb) (void * handle, int32_t argc, const char * argv[]);
  uint32_t argc;
  const char * cmd;
  const char * argv;
} CliMethodStructType;

static CliMethodStructType stCliCommandsList[] = {""")

for x in cli_cmds_list:
  id=x.C
  cmd = x.P.strip('\n').strip(' ')
  args = x.A.strip('\n').strip(' ')
  argi = x.A.count(' ')
  if not argi:
    buf =  """\n    {0x%08x, NULL, %d, "%s", NULL},""" % (id, argi, cmd)
  else:
    buf =  """\n    {0x%08x, NULL, %d, "%s", "%s"},""" % (id, argi, cmd, args)
  f.write(buf)

f.write("\n};\n\n")
f.close()
