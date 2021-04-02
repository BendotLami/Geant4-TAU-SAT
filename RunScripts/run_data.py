import os
import sys
import subprocess

MOTHER_FOLDER = '/runs/'
INPUT_FILE_NAME = 'input.txt'
GEANT_EXE_LOCATION = '/storage/t3_data/benl/project/LXe_silicon_25_2/build/LXe'


def make_dir(path):
    try:
        os.mkdir(path)
    except FileExistsError:
        pass


if __name__ == "__main__":
    save_path = sys.argv[1]
    if os.path.exists(save_path):
        os.chdir(save_path)
        MOTHER_FOLDER = save_path + MOTHER_FOLDER
    else:
        exit(1)

    for run_dir, _, filename in os.walk(os.getcwd()):
        if filename == [INPUT_FILE_NAME]:
            os.chdir(run_dir)
            with open("run_stdout.txt", 'w') as out_fd:
                subprocess.run([GEANT_EXE_LOCATION, INPUT_FILE_NAME], stdout=out_fd)

