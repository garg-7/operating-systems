import os
import subprocess
import matplotlib.pyplot as plt
from time import sleep
import numpy as np


def printer(results, title, N):
    # do the printing
    x = np.arange(10, 51, 10)
    times = ["Waiting", "Turnaround", "Response"]
    for i, t in enumerate(times):
        y1 = []
        y2 = []
        y3 = []
        y4 = []
        y5 = []
        for n in N:
            y1.append(results[n]["FCFS"][i])
            y2.append(results[n]["NSJF"][i])
            y3.append(results[n]["PSJF"][i])
            y4.append(results[n]["RR"][i])
            y5.append(results[n]["P"][i])
            # print(results[n]["FCFS"][0], end = " ")
        plt.xlabel("Number of Processes (N)")
        plt.ylabel(f"{title} {t} Time ({title[0]}{t[0]}T)")
        # plt.yticks(np.arange(min(min(y1),min(y2),min(y3),min(y4),min(y5)), max(max(y1),max(y2),max(y3),max(y4),max(y5)), 10))
        plt.plot(x, y1, label="FCFS")
        plt.plot(x, y2, label="NSJF")
        plt.plot(x, y3, label="PSJF")
        plt.plot(x, y4, label="RR")
        plt.plot(x, y5, label="P")
        plt.legend(loc='best')
        plt.show()
# loop over N values


def compiler():
    # compile both the generator and simulator files
    gen_compile = subprocess.Popen("gcc -o gen generator.c -lm".split(' '), stdout=subprocess.PIPE)
    o, e = gen_compile.communicate()

    if o:
        print(o.decode())
        exit(1)
    if e:
        print(e.decode())
        exit(1)

    sim_compile = subprocess.Popen("gcc -o sim simulate.c".split(' '), stdout=subprocess.PIPE)
    o, e = sim_compile.communicate()

    if o:
        print(o.decode())
        exit(1)
    if e:
        print(e.decode())
        exit(1)

    print("Generator and Simulator compiled successfully.")

def processor():

    N = [10, 20, 30, 40, 50]
    methods = ["FCFS", "NSJF", "PSJF", "RR", "P"]

    avg_results = {}
    max_results = {}
    min_results = {}

    for n in N:
        # ten times for every value of N
        avg_results[n] = {}
        max_results[n] = {}
        min_results[n] = {}
        trial_results = {}
        for m in methods:
            trial_results[m] = {}
        for i in range(10):
            # first generate the data
            generator = subprocess.Popen(["./gen", str(n), "2"], stdout=subprocess.PIPE)
            res, err = generator.communicate()
            sleep(1)
            # print(len(res.decode().strip().split('\n')))
            
            print(f"[{i+1}/10] Data generated for {n} processes.")
            # now call the methods one by one
            for m in methods:
                # print(f"Method: {m}")
                simulator = subprocess.Popen(["./sim", m], stdout=subprocess.PIPE)
                res, err = simulator.communicate()
                lines = res.decode().strip().split('\n')
                method_results = []
                for l in lines:
                    if l.startswith('Average'):
                        prop = l.split(":")[0].strip()
                        val = l.split(":")[1].strip().split(' ')[0]
                        # print(prop, ":", val)
                        method_results.append(val)
                trial_results[m][i] = method_results
            print(f"[{i+1}/10] Results obtained for all methods.")

        for m in trial_results.keys():
            waiting = 0
            response = 0
            turnaround = 0
            w_list = []
            t_list = []
            r_list = []
            for i in range(10):
                waiting += float(trial_results[m][i][0])
                turnaround += float(trial_results[m][i][1])
                response += float(trial_results[m][i][2])
                w_list.append(float(trial_results[m][i][0]))
                t_list.append(float(trial_results[m][i][1]))
                r_list.append(float(trial_results[m][i][2]))
            waiting /= 10
            response /= 10
            turnaround /= 10
            max_results[n][m] = [max(w_list), max(t_list), max(r_list)]
            min_results[n][m] = [min(w_list), min(t_list), min(r_list)]
            avg_results[n][m] = [waiting, turnaround, response]

    for n in avg_results:
        print(f"Number of processes: {n}")
        for m in methods:
            print(m,":",f"\tWaiting: Avg={avg_results[n][m][0]:.2f} Max={max_results[n][m][0]:.2f} Min={min_results[n][m][0]:.2f}")
            print(f"\tTurnaround: Avg={avg_results[n][m][1]:.2f} Max={max_results[n][m][1]:.2f} Min={min_results[n][m][1]:.2f}")
            print(f"\tResponse: Avg={avg_results[n][m][2]:.2f} Max={max_results[n][m][2]:.2f} Min={min_results[n][m][2]:.2f}")
        print()
    printer(max_results, "Maximum", N)
    printer(min_results, "Minimum", N)
    printer(avg_results, "Average", N)

if __name__ == '__main__':
    compiler()
    processor()
