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


def send_file(conn, filename, out):
    print(f'Sending {filename} to FTP path "{conn.pwd()}/{out}"')
    conn.storbinary(f'STOR {out}', open(filename, 'rb'))


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

    ftp.login() # anonymous logon

    # Ensure required directories exist on the console
    ensuredirectory(ftp, '/atmosphere', 'contents')
    ensuredirectory(ftp, '/atmosphere/contents', program_id)
    ensuredirectory(ftp, f'/atmosphere/contents/{program_id}', 'exefs')

    ftp.cwd(f'/atmosphere/contents/{program_id}/exefs')
    ##print(f'current working directory on ftp site is {ftp.pwd()}')

    # Send Gucci Mane
    send_file(ftp, './skyline.nso', f'./{subsdk_name}')
    send_file(ftp, f'./{code_name}.npdm', f'./main.npdm')
