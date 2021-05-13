import os
import sys
import csv

MOTHER_FOLDER = "/runs/"
INPUT_FILE_NAME = "input.txt"
OUTPUT_FILE_NAME = "/output" # should match what geant4 expects

def make_dir(path):
    try:
        os.mkdir(path)
    except FileExistsError:
        pass

def vec_to_str(in_str):
    return "{} {} {}".format(*in_str)

def particle_name(particle, ion):
    if particle != "ion":
        return particle
    return "{} - {}".format(particle, ion)


def create_input_file(filepath, particle: str, ion: str, energy: str,
                      direction: str, position: str, beamOn: int, position_unit: str = "cm", energy_unit: str = "MeV"):
    with open(filepath + "/" + INPUT_FILE_NAME, 'w') as f:
        f.write("/run/initialize\n")
        f.write("/gun/particle {}\n".format(particle))
        if particle == "ion":
            f.write("/gun/ion {}\n".format(ion))
        f.write("/gun/energy {} {}\n".format(energy, energy_unit))
        f.write("/gun/position {} {}\n".format(position, position_unit))  # 0 0 0 m (x,y,z, unit)
        f.write("/gun/direction {}\n".format(direction)) # 0 0 0
        for _ in range(beamOn):
            f.write("/run/beamOn 1\n")
        make_dir(filepath + OUTPUT_FILE_NAME)


def create_runs():
    energies = [0.5, 0.75, 1, 10, 50, 100, 250, 500, 750, 1000,
                2000, 4000, 5000, 7500, 10000, 100000]  # in MeV
    positions = [(0, 0, -4)]
    directions = [(0, 0, 1)]
    particles = ["mu+", "e-"] # for now ignoring ion
    ions = [(2, 4, 2), (6, 12, 6), (8, 16, 8), (26, 52, 26)]
    beamOn = 10 # number of particles to shoot

    with open("mapping.csv", 'w', newline='') as map_file:
        run_writer = csv.writer(map_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        run_writer.writerow(['Folder Number', 'Particle', 'Energy', 'Position', 'Direction'])
        i = 0

        for particle in particles:
            subfolder_subname = MOTHER_FOLDER + particle + "/"
            make_dir(subfolder_subname)
            ions_lst = [(0, 0, 0)]
            if particle == "ion":
                ions_lst = ions
            for ion in ions_lst:
                for energy in energies:
                    for pos in positions:
                        for direct in directions:
                            i+=1
                            run_writer.writerow([i, particle_name(particle, ion), energy, pos, direct])
                            subfolder_name = subfolder_subname + str(i)
                            make_dir(subfolder_name)
                            create_input_file(subfolder_name, particle, vec_to_str(ion), str(energy), vec_to_str(direct), vec_to_str(pos), beamOn)

if __name__=="__main__":
    save_path = sys.argv[1]
    if os.path.exists(save_path):
        os.chdir(save_path)
        MOTHER_FOLDER = save_path + MOTHER_FOLDER
    else:
        exit(1)

    make_dir(MOTHER_FOLDER)
    create_runs()
