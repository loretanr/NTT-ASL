import sys, itertools, os, math, random
from matplotlib import rc
rc('text', usetex=True) # this is if you want to use latex to print text. If you do you can create strings that go on labels or titles like this for example (with an r in front): r"$n=$ " + str(int(n))
from numpy import *
from pylab import *
from matplotlib.font_manager import FontProperties
import matplotlib.pyplot as plt
import matplotlib.lines as lns

   
background_color = '#eeeeee' 
grid_color = 'white' #FAFAF7'
matplotlib.rc('axes', facecolor = background_color)
matplotlib.rc('axes', edgecolor = grid_color)
matplotlib.rc('axes', linewidth = 1.2)
matplotlib.rc('axes', grid = True )
matplotlib.rc('axes', axisbelow = True)
matplotlib.rc('grid',color = grid_color)
matplotlib.rc('grid',linestyle='-' )
matplotlib.rc('grid',linewidth=0.7 )
#matplotlib.rc('xtick.major',size =0 )
#matplotlib.rc('xtick.minor',size =0 )
#matplotlib.rc('ytick.major',size =0 )
#matplotlib.rc('ytick.minor',size =0 )
matplotlib.rc('xtick', labelsize=14) 
matplotlib.rc('ytick', labelsize=14)



X_MIN=0.03125*3
X_MAX=32
Y_MIN=0.03125*3
Y_MAX=32
PEAK_PERF=5.0
PEAK_PERF2=20.0
ASPECT_RATIO=1

fig = plt.figure()
ax = fig.add_subplot(111, aspect = ASPECT_RATIO)


# formatting
ax.set_title("Machine Roofline Plot",fontsize=22,fontweight='bold')
ax.set_xlabel("Operational Intensity [Flop/Byte]", fontsize=17)
ax.set_ylabel("Performance [Flop/Cycle]", fontsize=17)
ax.set_yscale('log', basey=2)
ax.set_xscale('log', basex=2)
ax.axis([X_MIN,X_MAX,Y_MIN,Y_MAX])

# double the amount of ticks
def double_tic(tics):
    doubles = [2*elem for elem in tics]
    endres = [x for x in itertools.chain.from_iterable(itertools.izip_longest(tics,doubles)) if x]
    return endres[:-1]

ax.set_xticks(double_tic(ax.get_xticks()))
ax.set_yticks(double_tic(ax.get_yticks()))


# Horizontal lines
ax.axhline(y=PEAK_PERF, linewidth=1, color='black')
ax.text(8, PEAK_PERF+0.1, "Peak Performance no-vec ("+str(PEAK_PERF)+" F/C)", fontsize=15)
ax.axhline(y=PEAK_PERF2, linewidth=1, color='black')
ax.text(8, PEAK_PERF2+0.5, "Peak Performance SIMD ("+str(PEAK_PERF2)+" F/C)", fontsize=15)

# diagonal
x = np.linspace(0, X_MAX, X_MAX)
y = x * 32
ax.plot(x, y, linewidth=1, color='black')
l2 = array((0.01,0.01))
angle = 45*(ASPECT_RATIO + 0.05)
trans_angle = gca().transData.transform_angles(array((angle,)),l2.reshape((1,2)))[0]
ax.text(X_MIN/5,1,'Bandwidth',fontsize=15,rotation=trans_angle)

# intersection points
xc,yc = [0.15625, 0.625],[5,20]
ax.plot(xc[0],yc[0], ' o', color='red', alpha= 0.95, label='ridge = 5/32') 
ax.plot(xc[1],yc[1], ' o', color='red', alpha = 0.95, label='ridge = 20/32')
ax.annotate('ridge = 5/32',xy=(xc[0], yc[0]), xycoords='data',xytext=(+3, -15), textcoords='offset points', fontsize=12, color='red')
ax.annotate('ridge = 20/32',xy=(xc[1],yc[1]), xycoords='data',xytext=(+3, -15), textcoords='offset points', fontsize=12, color='red')

# Ci points
xc,yc = 0.3125, [3,2,5]
ax.plot(xc,yc[0], ' o', color='blue', alpha= 0.95, label='C1') 
ax.plot(xc,yc[1], ' o', color='blue', alpha = 0.95, label='C2')
ax.plot(xc,yc[2], ' o', color='blue', alpha = 0.95, label='C3')
ax.annotate('C1',xy=(xc, yc[0]), xycoords='data',xytext=(-13, -13), textcoords='offset points', fontsize=12, color='blue')
ax.annotate('C2',xy=(xc,yc[1]), xycoords='data',xytext=(-13, -13), textcoords='offset points', fontsize=12, color='blue')
ax.annotate('C3',xy=(xc,yc[2]), xycoords='data',xytext=(-13, +3), textcoords='offset points', fontsize=12, color='blue')

# Di points
xc,yc = 0.078125, [3,2,5]
ax.plot(xc,2.5, ' o', color='green', alpha= 0.95, label='D1 \& D3') 
ax.plot(xc,yc[1], ' o', color='green', alpha = 0.95, label='D2')
ax.annotate('D1 \& D3',xy=(xc, 2.5), xycoords='data',xytext=(-50, +2), textcoords='offset points', fontsize=12, color='green')
ax.annotate('D2',xy=(xc,yc[1]), xycoords='data',xytext=(-13, -13), textcoords='offset points', fontsize=12, color='green')


plt.show()
