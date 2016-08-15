#! /usr/bin/env python

from lab.parser import Parser

parser = Parser()
parser.add_pattern('pdb_generation_time', 'Pattern generation \(Edelkamp\) time: (.+)s', required=False, type=float)

parser.parse()
