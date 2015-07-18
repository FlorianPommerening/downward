#! /usr/bin/env python

from lab.parser import Parser


class CustomParser(Parser):
    def __init__(self):
        Parser.__init__(self)
        self.add_pattern(
            "successor_generator_time",
            "Building successor generator...done! \[t=(.+)s\]",
            required=False,
            type=float)
        self.add_pattern(
            "successor_generator_nodes",
            r"Successor generator has (\d+) nodes \[\d+ inner nodes, \d+ leaf nodes, \d+ empty nodes\]\.",
            required=False,
            type=int)
        self.add_pattern(
            "successor_generator_inner_nodes",
            r"Successor generator has \d+ nodes \[(\d+) inner nodes, \d+ leaf nodes, \d+ empty nodes\]\.",
            required=False,
            type=int)
        self.add_pattern(
            "successor_generator_leaf_nodes",
            r"Successor generator has \d+ nodes \[\d+ inner nodes, (\d+) leaf nodes, \d+ empty nodes\]\.",
            required=False,
            type=int)
        self.add_pattern(
            "successor_generator_empty_nodes",
            r"Successor generator has \d+ nodes \[\d+ inner nodes, \d+ leaf nodes, (\d+) empty nodes\]\.",
            required=False,
            type=int)


if __name__ == "__main__":
    parser = CustomParser()
    print "Running custom parser"
    parser.parse()
