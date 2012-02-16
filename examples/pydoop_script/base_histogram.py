
# Count the base frequency in sequencing data (in SAM format).
# Input:  file in SAM format
# Output:  tab-separated (base,count) pairs.

def mapper(k, samrecord, writer):
	seq = samrecord.split("\t", 10)[9]
	for c in seq:
		writer.emit(c, 1)
	writer.count("bases", len(seq))

def reducer(key, ivalue, writer):
	writer.emit(key, sum(map(int, ivalue)))
