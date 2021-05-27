# Author:  shadowninja108
# Copyright 2019 (c) The Starlight Project


from ftplib import FTP
import os
import sys


def listdirs(connection, _path):
    file_list, dirs, nondirs = [], [], []
    try:
        connection.cwd(_path)
    except:
        return []

    connection.retrlines('LIST', lambda x: file_list.append(x.split()))
    for info in file_list:
        ls_type, name = info[0], info[-1]
        if ls_type.startswith('d'):
            dirs.append(name)
        else:
            nondirs.append(name)
    return dirs


def ensuredirectory(connection, root, path):
    print(f"Ensuring {os.path.join(root, path)} exists...")
    if path not in listdirs(connection, root):
        connection.mkd(f'{root}/{path}')


FTPD_PORT = 5000


if __name__ == "__main__":
    consoleIP = sys.argv[1]
    if '.' not in consoleIP:
        print(sys.argv[0],
              "ERROR: Please specify with `IP=[Your console's IP]`")
        sys.exit(-1)

    program_id = sys.argv[2]
    code_name = sys.argv[3]
    subsdk_name = sys.argv[4]

    curDir = os.curdir

    ftp = FTP()
    print(f'Connecting to {consoleIP}... ', end='')
    ftp.connect(consoleIP, FTPD_PORT)
    print('Connected!')

    ensuredirectory(ftp, '/atmosphere', 'contents')
    ensuredirectory(ftp, '/atmosphere/contents', program_id)
    ensuredirectory(ftp, f'/atmosphere/contents/{program_id}', 'exefs')

    binaryPath = f'{os.path.basename(os.getcwd())}.nso'
    print(binaryPath)
    if os.path.isfile(binaryPath):
        sdPath = f'/atmosphere/contents/{program_id}/exefs/{subsdk_name}'
        print(f'Sending {sdPath}')
        ftp.storbinary(f'STOR {sdPath}', open(binaryPath, 'rb'))

        metaPath = f'{code_name}.npdm'
        sdPath = f'/atmosphere/contents/{program_id}/exefs/main.npdm'
        print(f'Sending {sdPath}')
        ftp.storbinary(f'STOR {sdPath}', open(metaPath, 'rb'))
