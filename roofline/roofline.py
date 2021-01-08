import csv
import sys
import math
import os
import collections


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


#flag = sys.argv[1]

flags = "-O3 -march=native"

results = collections.OrderedDict()
functions = list()
list_of_filenames = list()
plot_indexes = set()
smaller_font_size = 12
font_size = 14

# load csv file for roofline analysis
with open("roofline_results/roofline.csv") as file:
    table = csv.DictReader(file, delimiter=",")
    table = list(table)
    for row in table:
        function_name = row["function"]
        if function_name not in results:
            results[function_name] = collections.OrderedDict()
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

    all_types[function] = [{"list": intops,  "ylabel": "Performance [intops/cycle]", "label": "intops"}]

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
    plt.plot(x_values, values_to_plot, color=colors[j], label=function, marker='o', linewidth=.8, markersize=4)

    # annotate first points
    plt.annotate("32", xy=(x_values[0],values_to_plot[0]), color=colors[j], fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')
    # annotate last points
    plt.annotate("2M", xy=(x_values[-1],values_to_plot[-1]), color=colors[j], fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')
    middle_x = int(len(x_values)/2)
    middle_y = int(len(values_to_plot)/2)
    if function == "Best AVX":
        plt.annotate("Best Vector", xy=(x_values[middle_x], values_to_plot[middle_y]),
                     color=colors[j], fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')
    else:
        plt.annotate(function, xy=(x_values[middle_x], values_to_plot[middle_y]),
                     color=colors[j], fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')

    j = j + 1


# Set x axis limits from the lowest to the highest value of n
plt.xlim(0.01, 1.1 * float(max(all_x_values)))
plt.ylim(0.01, 80)

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

plt.xlabel("Operational Intensity [intops/byte]", fontsize=font_size, color="dimgrey")
# plt.ylabel(all_types[functions[0]][type_to_plot]["ylabel"], fontsize=font_size, rotation=0, position=(1,1), ha="left")
plt.xscale("log")
plt.yscale("log")

plt.grid(b=True, which="both", axis="both")

# plt.legend()
plt.title("NTT Radix 4 - Roofline analysis", loc="left", fontsize=20, y=1.1, va="top")
plt.title("{}\nL3: {}".format("Intel Core i7-7700", get_cpu_info()["l3_cache_size"]),
          loc="right", fontsize=font_size, color="black", y=1.1, va="top")
plt.suptitle(all_types[functions[0]][type_to_plot]["ylabel"],
             fontsize=font_size, rotation=0, position=(0.065, 0.92), ha="left", color="dimgrey")

"""filename = data_location + "benchmark_{}_{}_{}.pdf".format("".join(sys.argv[2:]),
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
l1_bw = 298.29  # measured L1 bandwidth (intel advisor)
l2_bw = 96.41  # measured L2 bandwidth (intel advisor)
l3_bw = 59.14  # measured L3 bandwidth (intel advisor)
dram_bw = 35.76  # theoretical peak (DRAM) bandwidth for i7 7700
actual_bw = 19.6329  # measured STREAM speed
freq = 3.6

from pylab import *
l2 = array((0.01, 0.01))
angle = 41.5
trans_angle = gca().transData.transform_angles(array((angle,)), l2.reshape((1, 2)))[0]

# Diagonal bandwidth lines
y4 = x * (l1_bw / freq)
plt.plot(x, y4, color="black", label="L1_bandwidth_rudolf", linewidth=.5, linestyle='dashed')
plt.text(0.011, 1.041, 'L1 Bandwidth {} GB/s'.format(round(l1_bw, 2)), fontsize=smaller_font_size, rotation=trans_angle, ha="left")

y3 = x * (l2_bw / freq)
plt.plot(x, y3, color="black", label="L2_bandwidth_rudolf", linewidth=.5, linestyle='dashed')
plt.text(0.011, 0.345, 'L2 Bandwidth {} GB/s'.format(round(l2_bw, 2)), fontsize=smaller_font_size, rotation=trans_angle, ha="left")

y2 = x * (l3_bw / freq)
plt.plot(x, y2, color="black", label="L3_bandwidth_rudolf", linewidth=.5, linestyle='dashed')
plt.text(0.011, 0.205, 'L3 Bandwidth {} GB/s'.format(round(l3_bw, 2)), fontsize=smaller_font_size, rotation=trans_angle, ha="left")

y = x * (dram_bw / freq)
plt.plot(x, y, color="black", label="DRAM_bandwidth_rudolf", linewidth=1.0)
plt.text(0.011, 0.13, 'DRAM Bandwidth {} GB/s'.format(round(dram_bw, 2)), fontsize=smaller_font_size, rotation=trans_angle, ha="left")

y5 = x * (actual_bw / freq)
plt.plot(x, y5, color="black", label="STREAM_bandwidth_rudolf", linewidth=.5, linestyle='dashed')
plt.text(0.011, 0.071, 'Measured Peak Bandwidth (STREAM) {} GB/s'.format(round(actual_bw, 2)),
         fontsize=smaller_font_size, rotation=trans_angle, ha="left")

# Horizontal lines
p_scalar = [[-10000, 10000], [4, 4]]  # Theoretical Int Scalar Add Peak
plt.plot(p_scalar[0], p_scalar[1], color="black", label="Peak Rudolf novec", marker='o', linewidth=.5)
plt.text(12.35, 4.2, "Scalar Int Add Peak", fontsize=smaller_font_size, ha="right")

p_avx = [[-10000, 10000], [13, 13]]  # Theoretical Int Vector Add Peak
plt.plot(p_avx[0], p_avx[1], color="black", label="Peak Rudolf avx", marker='o', linewidth=.5, linestyle='dashed')
plt.text(12.35, 17, "Vector Int Add Peak", fontsize=smaller_font_size, ha="right")

fig.axes[0].tick_params(axis="both", which="both", labelsize=14, color="black")
plt.tight_layout()
filename = "roofline_results/roofline.pdf"
plt.savefig(filename)

plt.show()