import os
import glob

# response = input("This will remove all log files.... Continue? y/n\n")

# if response == 'y':
#     exp1 = glob.glob('../logs/exp1/*')
#     exp2 = glob.glob('../logs/exp2/*')
#     for f1 in exp1:
#         os.remove(f1)
#     for f2 in exp2:
#         os.remove(f2)
# else:
#     quit()

exp1 = glob.glob('../logs/exp1/*')
exp2 = glob.glob('../logs/exp2/*')
for f1 in exp1:
    os.remove(f1)
for f2 in exp2:
    os.remove(f2)
