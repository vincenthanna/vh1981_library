import re

regstr = '[DE]F+[[DE]F+|G]'
p = re.compile(regstr)

print(re.findall(regstr, "DFFEFFFG"))
