# Author:  shadowninja108
# Copyright 2019 (c) The Starlight Project

import argparse
import os
import sys
import json
from ftplib import FTP

ftpConnection = None

def ListDirs(_path):
    file_list, dirs, nondirs = [], [], []
    try:
        ftpConnection.cwd(_path)
    except:
        return []

    ftpConnection.retrlines('LIST', lambda x: file_list.append(x.split()))

    for info in file_list:
        ls_type, name = info[0], info[-1]
        if ls_type.startswith('d'):
            dirs.append(name)
        else:
            nondirs.append(name)
    return dirs


def EnsureDirectory(root, path):
    print(f"Ensuring {os.path.join(root, path)} exists...")

    if path not in ListDirs(root):
        ftpSite.mkd(f'{root}/{path}')


def SendFile(filename, out):
    print(f'Sending {filename} to FTP path "{ftpConnection.pwd()}/{out}"')
    ftpConnection.storbinary(f'STOR {out}', open(filename, 'rb'))


# bail (printing a message beforehand) and exit
def Bail(message):
    print(f'Error: {message}')
    sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Send and update bf2mods binary output to Switch console, via FTP.')

    # stuff we gather from
    parser.add_argument('--json', dest='path', required=True, type=str, help='path to npdmtool JSON file to gather information from')
    parser.add_argument('--subsdk', dest='subsdk_name', required=False, type=str, default="subsdk1", help='subsdk name (default \'subsdk1\')')

    # ftp site arguments
    parser.add_argument('--switchip', dest='ip', required=True, type=str, help='Switch IP')
    parser.add_argument('--switchport', dest='port', required=False, type=int, default=5000, help='Switch FTP port (default 5000)')

    args = parser.parse_args()

    if '.' not in args.ip:
        Bail("Invalid Switch IP address provided")

    if not os.path.exists(args.path):
        Bail("JSON file doesn't exist")

    file = open(args.path, mode='r')

    # additionaly if it doesn't have json data, it probably shouldn't be used either, bail there too.
    try:
        jsondata = json.loads(file.read())
    except:
        Bail("JSON file contains invalid data")

    file.close()

    titleid = jsondata['title_id'][2:].upper()

    print(f"Gathered title id is {titleid}")

    ftpConnection = FTP()
    print(f'Connecting to Switch console @ {args.ip}:{args.port} ...', end='')
    ftpConnection.connect(args.ip, args.port)

    print('Connected!')

    ftpConnection.login()

    # Ensure required directories exist on the console; if not, create them
    EnsureDirectory('/atmosphere', 'contents')
    EnsureDirectory('/atmosphere/contents', program_id)
    EnsureDirectory(f'/atmosphere/contents/{titleid}', 'exefs')

    ftpConnection.cwd(f'/atmosphere/contents/{titleid}/exefs')

    # Send bf2mods to the console
    SendFile('./bf2mods.nso', f'{args.subsdk_name}')
    SendFile('./main.npdm', 'main.npdm')
