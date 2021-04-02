import os
import sys
import subprocess
import re

MOTHER_FOLDER = '/runs/'
INPUT_FILE_NAME = 'input.txt'
OUTPUT_FILE_NAME = 'test.txt'
GEANT_EXE_LOCATION = '/storage/t3_data/benl/project/LXe_silicon_25_2/build/LXe'

NUMBER_OF_SLABS = 4


def make_dir(path):
    try:
        os.mkdir(path)
    except FileExistsError:
        pass


def get_locations(content, i, res):
    old_len = len(res)
    NUMBER_OF_SLABS -= 1
    while i < len(content):
        i += 1
        if "Enter Location" in content[i] or "Exit Location" in content[i]:
            res.append(re.findall(r"[-+]?\d*\.\d+|\d+", content[i]))
        else if "Particle Volume" in content[i] or "Number of" in content[i]:
            break
    if len(res) < old_len+2: # in case the particle stopped in this slab
        res.append(['-','-','-'])
    return i


def add_missing_slabs(res):
    while NUMBER_OF_SLABS < 0:
        res.append(['-','-','-'])
        res.append(['-','-','-'])
        NUMBER_OF_SLABS -= 1


def get_particle_count(line, loc):
    lst_nums = re.findall(r"[-+]?\d*\.\d+|\d+", line)
    return lst_nums[loc]


def get_scientific_number(line):
    match_number = re.compile('-?\ *[0-9]+\.?[0-9]*(?:[Ee]\ *[-+]?\ *[0-9]+)?')
    final_list = [float(x) for x in re.findall(match_number, line)]
    return final_list[0]

def get_run_data(run_dir):
    res = list()
    with open("{}/{}".format(run_dir, OUTPUT_FILE_NAME), 'r') as output_file:
        file_lines = output_file.readlines()
    content = [[line.rstrip('\n') for line in file_lines]
    i = 0
    while i < len(content):
        # Getting locations
        if "Particle Volume" in content[i]:
            i = get_locations(content, i, res)
            continue
        # Check if there are untouchable slabs
        add_missing_slabs(res)
        else if "Number of hits per event:" in content[i]:
            res.append(get_scientific_number(content[i]))
        else if "Silicon slab no." in content[i]:
            res.append(get_particle_count(content[i], 1))
        else if "Scintillator" in content[i]:
            res.append(get_particle_count(content[i], 2))
        i += 1


if __name__ == "__main__":
    save_path = sys.argv[1]
    if os.path.exists(save_path):
        os.chdir(save_path)
        MOTHER_FOLDER = save_path + MOTHER_FOLDER
    else:
        exit(1)

    simulation_data = list()
    for run_dir, _, filename in os.walk(os.getcwd()):
        if filename == [INPUT_FILE_NAME]:
            os.chdir(run_dir)
            with open("run_stdout.txt", 'w') as out_fd:
                subprocess.run([GEANT_EXE_LOCATION, INPUT_FILE_NAME], stdout=out_fd)
            simulation_data.append(get_run_data(run_dir))
            print('Debug: extracted the following data: \n {}'.format(simulation_data[len(simulation_data)-1]))
    
    with open("{}mapping.csv".format(MOTHER_FOLDER),'r') as csvinput:
        with open("{}final_results.csv".format(MOTHER_FOLDER), 'w') as csvoutput:
            writer = csv.writer(csvoutput, lineterminator='\n')
            reader = csv.reader(csvinput)

            output_lines = []
            row = next(reader)
            row.append(['Silicon_2 enter location', 'Silicon_2 exit location', 'Silicon_1 enter location', 'Silicon_1 exit location', 'Scint_1 enter location', 'Scint_1 exit location', 'Scint_2 enter location', 'Scint_2 exit location', 'Number of photons created', 'Number of electron-hole pairs created', 'Photons absorbed in Scint_1 Top-Left', 'Photons absorbed in Scint_1 Bottom-Right', 'Photons absorbed in Scint_1 Top-Right', 'Photons absorbed in Scint_1 Bottom-Left', 'Photons absorbed in Scint_2 Top-Left', 'Photons absorbed in Scint_2 Bottom-Right', 'Photons absorbed in Scint_2 Top-Right', 'Photons absorbed in Scint_2 Bottom-Left'])
            output_lines.append(row)

            for i,row in enumerate(reader):
                row.extend(simulation_data)
                output_lines.append(row)

            writer.writerows(output_lines)

