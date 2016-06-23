#!/usr/bin/python
# Runs the taint analysis over a given set of articles.

import sys
import re
import os
import subprocess

if len(sys.argv) < 2:
	print "Usage: " + sys.argv[0] + " <article_list_file> <config_file>"


# Get the filename that contains a list of articles to run over
article_list = sys.argv[1]

# Get the configuration file
config_file = sys.argv[2]


# Open the article list file and populate an array of articles we will analyze
article_fd = open(article_list, "r")
articles = []

for line in article_fd:
	line = line.strip()
	articles.append(line)


# For each article in the list we need to use the configuration file to generate the necessary
# input files for the analysis.
for i in range(len(articles)):
	print articles[i]
	# Generate names of files to be creates for the given article name
	taint_filename = articles[i] + "_taint_input.txt"
	source_sink_filename = articles[i] + "_source_sink.txt"
	query_filename = articles[i] + "_query.txt"
	fields_filename = articles[i] + "_fields.rtf"
	sinks_filename = articles[i] + "_sinks.txt"
	mediator_filename = articles[i] + "_mediator.txt"

	# Open these files with "w" to overwrite any pre-existing files
	taint_fd = open(taint_filename, "w")
	source_sink_fd = open(source_sink_filename, "w")
	query_fd = open(query_filename, "w")	
	fields_fd = open(fields_filename, "w")
	sinks_fd = open(sinks_filename, "w")
	mediator_fd = open(mediator_filename, "w")


	config_fd = open(config_file, "r")
	for line in config_fd:
		
		# Check if its the taint source line
		string = articles[i] + "_taint_source"
		if string in line:
			taint_source_raw = line.split(":")
			taint_source = taint_source_raw[1]
			taint_fd.write(taint_source)

		# Check if its a source sink line		
		string = articles[i] + "_source_sink"
		if string in line:
			print line
			source_sink_raw = line.split(":")
			source_sink = source_sink_raw[1]
			source_sink_fd.write(source_sink)

		# Check if its a query line
		string = articles[i] + "_query"
		if string in line:
			query_raw = line.split(":")
			query = query_raw[1]
			query_fd.write(query)

		# Check if its a field line
		string = articles[i] + "_fields"
		if string in line:
			field_raw = line.split(":")
			field = field_raw[1]
			fields_fd.write(field)

		# Check if its a sink line
		string = articles[i] + "_sinks"
		if string in line:
			sinks_raw = line.split(":")
			sinks = sinks_raw[1]
			sinks_fd.write(sinks)

		# Check if its a mediator line
		string = articles[i] + "_mediator"
		if string in line:
			mediators_raw = line.split(":")
			mediators = mediators_raw[1]
			mediator_fd.write(mediators)


# Now that our input files are generated for all articles we can actually run
# run the analysis for each.

primary_cmd = ""

for i in range(len(articles)):

	# Generate names of files to be creates for the given article name
	taint_filename = articles[i] + "_taint_input.txt"
	source_sink_filename = articles[i] + "_source_sink.txt"
	query_filename = articles[i] + "_query.txt"
	field_filename = articles[i] + "_fields.rtf"
	sinks_filename = articles[i] + "_sinks.txt"
	mediator_filename = articles[i] + "_mediator.txt"
	article_filename = articles[i] + ".bc"

	# Setup names for output files for the subprocess module
	article_output = articles[i] + ".ll"
	article_error = articles[i] + "_analysis_output"

	# Open the files for the subprocess module
	output_fd = open(article_output, "w")
	error_fd = open(article_error, "w")

	cmd = ['opt', '-load', '../../Debug+Asserts/lib/LLVMDataStructure.so', '-load', '../../Debug+Asserts/lib/TaintFlow.so', '-TaintS', '-time-passes', '-full=false', '-includeAllInstsInDepGraph=true', '-taintSource=' + taint_filename, '-fields=' + field_filename, '-query=' + query_filename, '-fullAnalysis=true', '-printall=true', article_filename, '-S']

	cmd_string = "opt -load ../../Debug+Asserts/lib/LLVMDataStructure.so -load ../../Debug+Asserts/lib/TaintFlow.so -TaintS -time-passes -full=false -includeAllInstsInDepGraph=true -sinks=" + sinks_filename + " -mediator=" + mediator_filename + " -fields=" + fields_filename + " -taintSource=" + taint_filename + " -query=" + query_filename + " -fullAnalysis=true -printall=true -S " + article_filename + " > " + articles[i] + ".ll" + " 2> " + article_error

	# Since the taint analysis doesn't take the source_sink as input we need to copy it to
	# the main file the information is held in.
	source_sink_primary = "source_sink.txt"
	copy_cmd = "cp " + source_sink_filename + " " + source_sink_primary
	
	# Run the analysis and hope for the best...
	#return_code = subprocess.call(cmd, stdout=output_fd, stderr=error_fd)
	#return_code = subprocess.call(cmd_string, shell=True)
	
	primary_cmd += copy_cmd + " ; " + cmd_string + " ; "
	
	#print cmd_string


print primary_cmd
	
	
