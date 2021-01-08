import csv
import sys
import math
import os

from cpuinfo import get_cpu_info
import numpy
import matplotlib.pyplot as plt

FUNCTION_STR = "function"
PLOT_INDEX_STR = "plot_index"
GRAPH_TITLE_STR = "graph_title"
FLAG_STR = "flag"
INTOPS_STR = "intops"
CYCLES_STR = "cycles"
TIMING_STR = "timing"
N_STR = "n"
type_to_plot = 0

smaller_font_size = 12
font_size = 14

data_location = "benchmark_results/"
flag_optimisation = "optimisation"
# Good looking plot design
plt.style.use("ggplot")
colors = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple", "tab:brown", "tab:pink", "tab:gray",
          "tab:purple", "tab:cyan", "yellow", "black", "lime"]


# Helper function for list of tuples sorting
def get_key(item):
    return item[0]


# compiler = sys.argv[1]
# version = sys.argv[2]
# flags = "".join(sys.argv[3:])
# compiler_and_version = compiler + " " + version
results = dict()
functions = list()
list_of_filenames = list()
plot_indexes = set()

# load csv benchmark file
# with open(data_location + "benchmarks_{}.csv".format("".join(sys.argv[2:]))) as file:

with open(data_location + "benchmarks.csv") as file:
    table = csv.DictReader(file, delimiter=",")
    table = list(table)
    for row in table:
        # function_name = row[FUNCTION_STR] + "_" + row[FLAG_STR] + "_" + str(row[PLOT_INDEX_STR])
        function_name = row[FUNCTION_STR] + "_" + str(row[PLOT_INDEX_STR])
        plot_indexes.add(int(row[PLOT_INDEX_STR]))
        if function_name not in results:
            results[function_name] = dict()
            functions.append(function_name)
        results[function_name][row[N_STR]] = {CYCLES_STR: float(row[CYCLES_STR]),
                                              TIMING_STR: float(row[TIMING_STR]),
                                              INTOPS_STR: float(row[INTOPS_STR])}

graph_titles = [
                "Radix 4 Recursive - Scalar",
                "Radix 4 Recursive - Vectorized",
                "Radix 2 Iterative - Scalar",
                "Radix 2 Iterative - Vectorized",
                "Overview"
                ]
x_values = list()
y_ticks = list()
x_tick_labels = list()
y_tick_labels = list()
values_to_plot = list()

all_types = dict()
for function in functions:

    cycles = list()
    runtime = list()
    intops = list()
    for key, value in results[function].items():
        temp_x = math.log(int(key), 2)
        cycles.append((temp_x, value[CYCLES_STR]))
        runtime.append((temp_x, value[TIMING_STR]))
        intops.append((temp_x, value[INTOPS_STR]))

    all_types[function] = [{"list": cycles,  "ylabel": "Runtime (cycles)", "label": "cycles"},
                           {"list": runtime, "ylabel": "Runtime (ms)", "label": "runtime"},
                           {"list": intops,  "ylabel": "Performance (intops/cycle)", "label": "intops"}]

for plot_index in plot_indexes:
    graph_title = graph_titles[plot_index]
    for type_to_plot in range(0, 3):
        j = 0
        all_y_values = list()
        fig = plt.figure(figsize=(10, 8), dpi=100)
        dgrey = "dimgrey"
        plt.axvline(12, color=dgrey, linewidth=0.7)
        plt.axvline(15, color=dgrey, linewidth=0.7)
        plt.axvline(20, color=dgrey, linewidth=0.7)
        for function in functions:
            if int(function[-1]) != plot_index:
                continue
            values_to_plot = all_types[function][type_to_plot]["list"]

            # sort values_to_plot according to x values
            values_to_plot = sorted(values_to_plot, key=get_key)

            # Split tuples into two lists
            # x_values: list of integers representing the x values
            # values_to_plot: list of floats representing the y values
            x_values = []
            for i in range(0, len(values_to_plot)):
                x_values.append(values_to_plot[i][0])
                values_to_plot[i] = values_to_plot[i][1]
            # Concatenate all y values so we can find the global maximum later
            all_y_values = all_y_values + values_to_plot

            # Add line to the plot for the function
            color_to_plot = colors[j]
            if function == "Baseline Scalar_0":
                xy = (x_values[-2], 0.08)
            elif function == "Baseline Vectorized_1":
                xy = (x_values[-2], 0.68)
            elif function == "Shifts_2":
                xy = (17, values_to_plot[-2])
            elif function == "Best Radix 2 Scalar_4":
                xy = (x_values[-2], 0.35)
            elif function == "Best Radix 4 Vectorized_4":
                xy = (x_values[-2], 1.17)
            else:
                xy = (x_values[-2], values_to_plot[-2])
            plt.plot(x_values, values_to_plot, color=color_to_plot,
                     label=function[0:len(function)-2], marker='o', linewidth=1.0)
            plt.annotate(function[:-2], xy=xy, ha="right", color=colors[j], fontsize=smaller_font_size,
                         xytext=(-8, 8), textcoords='offset points')
            j = j + 1

        # Set x axis limits from the lowest to the highest value of n
        plt.xlim(int(min(x_values)), int(max(x_values)))
        # Set x ticks to all values in x_values
        plt.xticks(x_values, list(str(int(item)) for item in x_values))

        # Generate a list of 7 numbers between 0.0 and the maximum y value + 10% for the y_ticks and y_limits
        top_y_value = 1.1*max(all_y_values)
        plt.ylim(0.0, top_y_value)
        y_values = list(round(item, 2) for item in numpy.linspace(0.0, top_y_value, num=7))
        if y_values[-1] > 100000:
            plt.yticks(y_values, list(str(int(item)) for item in y_values))
        else:
            plt.yticks(y_values, list(str(item) for item in y_values))

        fig.axes[0].spines['bottom'].set_color('black')
        plt.tick_params(axis="x")

        x_tick_labels = list()
        for x in x_values:
            if x % 2 == 1:
                x_tick_labels.append(r"$2^{%d}$" % int(x))
            else:
                x_tick_labels.append("")
        fig.axes[0].set_xticklabels(x_tick_labels)
        fig.axes[0].tick_params(axis="both", which="both", labelsize=14, color="black")
        plt.annotate("L1", xy=(12, top_y_value / 1.05), ha="right", color=dgrey, fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')
        plt.annotate("L2", xy=(15, top_y_value / 1.05), ha="right", color=dgrey, fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')
        plt.annotate("L3", xy=(20, top_y_value / 1.05), ha="right", color=dgrey, fontsize=smaller_font_size, xytext=(-8, 8), textcoords='offset points')

        plt.xlabel("Input size", fontsize=14, color="dimgrey")
        # plt.ylabel(all_types[functions[0]][type_to_plot]["ylabel"], fontsize=14, color="dimgrey")
        # plt.legend()
        plt.title("NTT - {}".format(graph_title), loc="left", fontsize=20, y=1.1, va="top")
        plt.title("{}\nL3: {}".format("Intel Core i7-7700", get_cpu_info()["l3_cache_size"]),
                  loc="right", fontsize=font_size, color="black", y=1.1, va="top")
        plt.suptitle(all_types[functions[0]][type_to_plot]["ylabel"],
                     fontsize=font_size, rotation=0, position=(0.065, 0.92), ha="left", color="dimgrey")

        filename = data_location + "benchmark_{}_{}_{}.pdf".format("".join(sys.argv[2:]),
                                                                   all_types[functions[0]][type_to_plot]["label"],
                                                                   plot_index)
        plt.tight_layout()
        plt.savefig(filename)
