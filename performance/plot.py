import numpy as np
import matplotlib as plt
import csv
import pandas as pd
import sys
file = "benchmark_results/benchmarks_{}.csv".format(''.join(sys.argv[1:]))
#data = pd.read_csv("benchmark_results/benchmarks_{}.csv".format(''.join(sys.argv[1:])), index_col='country')
df = pd.read_csv(file)[['function', 'n', 'cycles']]
functions = pd.DataFrame(columns=df['function'].unique(), index=range(1, 10))
functions = pd.DataFrame()


print(df['function'].unique())

for i in df['function'].unique():
    print(i)
    # functions[i] =df[df['function'] == i]['cycles']
    # functions.append(df[df['function'] == i]['cycles'], ignore_index=False)
    # print(df[df['function'] == i]['cycles'])
    functions = pd.concat([functions, df[df['function'] == i]['cycles'].reset_index()], axis=1)
print(functions)
