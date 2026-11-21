#!/usr/bin/env python3
import os
import shutil
import sys
import platform
import pre_qc

def make_quest():
    object_dir = 'object'
    pre_qc_dir = 'pre_qc'

    if os.path.exists(object_dir):
        shutil.rmtree(object_dir)
    os.makedirs(object_dir, exist_ok=True)

    if os.path.exists(pre_qc_dir):
        shutil.rmtree(pre_qc_dir)
    os.makedirs(pre_qc_dir, exist_ok=True)

    if platform.system() != "Windows":
        try:
            pass
        except:
            pass

    qc_bin = "./qc" if platform.system() != "Windows" else "qc.exe"

    if not os.path.exists('locale_list'):
        print("Error: locale_list not found.")
        return

    with open('locale_list', 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            
            is_pre_qced = pre_qc.run(line)
            
            if is_pre_qced:
                filename = os.path.join(pre_qc_dir, line)
            else:
                filename = line

            print(f"Compiling: {line}")
            
            exit_code = os.system(f"{qc_bin} {filename}")
            
            if exit_code != 0:
                print(f"Fehler beim Kompilieren von: {line}")
                if platform.system() != "Windows":
                    os.system('chmod -R 770 object')
                sys.exit(-1)

    if platform.system() != "Windows":
        os.system('chmod -R 770 object')
    
    print("Quests succesfully compiled.")

if __name__ == "__main__":
    make_quest()