#!/usr/bin/env python3

# Hacky script for xenomods packaging

import argparse
import os
import sys
import json

from shutil import copyfile
from pathlib import Path

# bail (printing a message beforehand) and exit
def Bail(message):
    print(f'Error: {message}')
    sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Script for packaging.')

    # stuff we gather from
    parser.add_argument('--codename', dest='codename', required=True, type=str, help='codename(s) of the build type')
    parser.add_argument('--subsdk', dest='subsdk_name', required=False, type=str, default="subsdk9", help='subsdk name (default \'subsdk9\')')

    args = parser.parse_args()

    for codename in args.codename.split(';'):
        print(f"Codename: {codename}")
        npdmPath = Path(os.getcwd()) / f"../npdm/{codename}.json"

        if not os.path.exists(npdmPath):
            Bail("NPDM JSON file doesn't exist")

        with open(npdmPath, mode='r') as file:
            # additionaly if it doesn't have json data, it probably shouldn't be used either, bail there too.
            try:
                jsondata = json.loads(file.read())
            except:
                Bail("NPDM JSON file contains invalid data")

        programid = jsondata['program_id'][2:].upper()

        print(f"Gathered program id is {programid}")

        amspath = Path(os.getcwd()) / 'pkg' / 'atmosphere' / 'contents' / programid / 'exefs'
        amspath.mkdir(parents=True)

        copyfile('xenomods.nso', str(amspath / args.subsdk_name))
        copyfile(f'{codename}.npdm', str(amspath / 'main.npdm'))

    print('Generated package data')
