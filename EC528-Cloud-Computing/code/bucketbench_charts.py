import matplotlib
import matplotlib.pyplot as plt
import numpy as np


labels = ['Run', 'Stop', 'Delete']
docker_means = [1332.11, 351.18, 108.44]
runc_means = [664.18, 7.38, 6.44]

x = np.arange(len(labels))  # the label locations
width = 0.35  # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(x - width/2, docker_means, width, label='Docker')
rects2 = ax.bar(x + width/2, runc_means, width, label='Runc')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('Mean')
ax.set_title('Comparison of Docker vs Runc')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()


def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


autolabel(rects1)
autolabel(rects2)

fig.tight_layout()

plt.show()