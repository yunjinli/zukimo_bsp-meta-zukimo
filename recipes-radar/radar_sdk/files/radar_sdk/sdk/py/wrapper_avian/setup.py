from setuptools.command.bdist_egg import bdist_egg as _bdist_egg
from setuptools import setup
import os, pathlib

if __name__ == "__main__":
    # Change to directory of script (required by cmake)
    os.chdir(pathlib.Path(__file__).resolve().parent)

    setup()
