import csv
import sys
import math
import os

from cpuinfo import get_cpu_info
import numpy
import matplotlib.pyplot as plt

FUNCTION_STR = "function"
PLOT_INDEX_STR = "plot_index"
FLAG_STR = "flag"
INTOPS_STR = "intops"
CYCLES_STR = "cycles"
TIMING_STR = "timing"
N_STR = "n"
type_to_plot = 0

# Good looking plot design
plt.style.use("ggplot")
colors = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple", "tab:brown", "tab:pink", "tab:gray",
          "tab:purple", "tab:cyan", "yellow", "black", "lime"]


# Helper function for list of tuples sorting
def get_key(item):
    return item[0]


#compiler = sys.argv[1]

results = dict()
functions = list()
list_of_filenames = list()
plot_indexes = set()

# load csv file for roofline analysis
with open("roofline.csv") as file:
    table = csv.DictReader(file, delimiter=",")
    table = list(table)
    for row in table:
        function_name = row["function"]
        if function_name not in results:
            results[function_name] = dict()
            functions.append(function_name)
        results[function_name][row["n"]] = {"intensity": float(row["intensity"]),
                                            "intops": float(row["intops"])}

x_values = list()
all_x_values = list()
all_y_values = list()
# y_ticks = list()
# x_tick_labels = list()
# y_tick_labels = list()
values_to_plot = list()

all_types = dict()
for function in functions:
    intops = list()
    for key, value in results[function].items():
        temp_x = value["intensity"]
        intops.append((temp_x, value["intops"]))
    # print(str(intops))

    all_types[function] = [{"list": intops,  "ylabel": "Performance (intops/cycle)", "label": "intops"}]

j = 0

fig = plt.figure(figsize=(11, 8), dpi=100)
for function in functions:
    values_to_plot = all_types[function][type_to_plot]["list"]

    # sort values_to_plot according to x values
    # values_to_plot = sorted(values_to_plot, key=get_key)

    # Split tuples into two lists
    # x_values: list of integers representing the x values
    # values_to_plot: list of floats representing the y values
    x_values = []
    for i in range(0, len(values_to_plot)):
        x_values.append(values_to_plot[i][0])
        values_to_plot[i] = values_to_plot[i][1]

    # Concatenate all y values so we can find the global maximum later
    all_y_values = all_y_values + values_to_plot
    all_x_values = all_x_values + x_values

    # Add line to the plot for the function
    plt.plot(x_values, values_to_plot, color=colors[j], label=function, marker='o', linewidth=1.0)
    j = j + 1



# Set x axis limits from the lowest to the highest value of n
plt.xlim(0.01, 1.1 * float(max(all_x_values)))

# Set x ticks to all values in x_values
# plt.xticks(x_values, list(str(int(item)) for item in x_values))

# Generate a list of 7 numbers between 0.0 and the maximum y value + 10% for the y_ticks and y_limits
# top_y_value = 1.1*max(all_y_values)
# plt.ylim(0.0, 5)

# y_values = list(round(item, 2) for item in numpy.linspace(0.0, top_y_value, num=7))
# if y_values[-1] > 100000:
#     plt.yticks(y_values, list(str(int(item)) for item in y_values))
# else:
#     plt.yticks(y_values, list(str(item) for item in y_values))

fig.axes[0].spines['bottom'].set_color('black')
plt.tick_params(axis="x")

plt.xlabel("Operational intensity", fontsize=10)
plt.ylabel(all_types[functions[0]][type_to_plot]["ylabel"], fontsize=10)

plt.xscale("log")
plt.yscale("log")

plt.legend()

plt.title("Number theoretic transform - $2^N$", loc="left", fontsize=20)
""" plt.title("\nCompiled using {}\n{}\nL2 Cache size: {}".format(compiler,
                                                                get_cpu_info()["brand"],
                                                                get_cpu_info()["l2_cache_size"]),
            loc="right", fontsize=10, color="grey") 

filename = data_location + "benchmark_{}_{}_{}.pdf".format("".join(sys.argv[2:]),
                                                            all_types[functions[0]][type_to_plot]["label"],
                                                            plot_index)
plt.savefig(filename)"""


# dummy diagonals
#plt.plot([0, 400], [0, 2000], color=colors[j + 1], label="BW scalar", marker='o', linewidth=1.0)
#plt.plot([0, 400], [0, 4000], color=colors[j + 2], label="BW vector", marker='o', linewidth=1.0)

# output from STREAM: (rudolf laptop)
#  Function    Best Rate MB/s  Avg time     Min time     Max time
#  Copy:            9855.2     0.016269     0.016235     0.016353
#  Scale:           9654.8     0.016599     0.016572     0.016642
#  Add:            11154.0     0.021563     0.021517     0.021626
#  Triad:          11078.3     0.021713     0.021664     0.021771

# Diagonal(s)
x = numpy.linspace(-100, 1000, 100)
y = x * 12.18 # theoretical peak bandwidth for i7 7600U @ 2.8Ghz
plt.plot(x,y,color=colors[j + 3], label="diag_rudolf", linewidth=1.0)

# Horizontal lines
perf_limit_rudolf_novec = [[-10000,10000],[5,5]]
perf_limit_rudolf_avx = [[-10000,10000],[20,20]]
plt.plot(perf_limit_rudolf_novec[0], perf_limit_rudolf_novec[1], color=colors[j + 1], label="Peak Rudolf novec", marker='o', linewidth=1.0)
plt.plot(perf_limit_rudolf_avx[0], perf_limit_rudolf_avx[1], color=colors[j + 2], label="Peak Rudolf avx", marker='o', linewidth=1.0)


plt.show()