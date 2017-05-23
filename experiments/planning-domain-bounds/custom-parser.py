#! /usr/bin/env python
import re
from lab.parser import Parser

def last_f_value(content, props):
    f_values = [int(f) for f in re.findall(r'f = (\d+) \[\d+ evaluated, \d+ expanded, t=.+s, \d+ KB]', content)]
    props['f_values'] = f_values
    if f_values:
        props['last_f_value'] = max(f_values)

class CustomParser(Parser):
    def __init__(self):
        Parser.__init__(self)
        self.add_function(last_f_value)


if __name__ == "__main__":
    parser = CustomParser()
    print "Running custom parser"
    parser.parse()
