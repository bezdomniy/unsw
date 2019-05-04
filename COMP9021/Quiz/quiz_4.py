# Uses Heath Nutrition and Population statistics, avalaible at
# http://datacatalog.worldbank.org, stored in the file HNP_Data.csv,
# assumed to be stored in the working directory.
# Prompts the user for an Indicator Name. If it exists and is associated with
# a numerical value for some countries or categories, for some the years 1960-2015,
# then finds out the maximum value, and outputs:
# - that value;
# - the years when that value was reached, from oldest to more recents years;
# - for each such year, the countries or categories for which that value was reached,
#   listed in lexicographic order.
# 
# Written by *** and Eric Martin for COMP9021


import sys
import os
import csv


filename = 'HNP_Data.csv'
if not os.path.exists(filename):
    print('There is no file named {} in the working directory, giving up...'.format(file_name))
    sys.exit()

indicator_of_interest = input('Enter an Indicator Name: ')

first_year = 1960
number_of_years = 56
max_value = None
countries_for_max_value_per_year = {}

with open(filename) as csvfile:
    in_file = csv.reader(csvfile)
    next(in_file, None)
    indicator_dict = {}

    for record in in_file:
        if record[2] not in indicator_dict:
            indicator_dict.update({record[2]:{record[0]:record[4:4+number_of_years]}})
        else:
            indicator_dict[record[2]].update({record[0]:record[4:4+number_of_years]})

    try:
        max_value_search=0
        for country in sorted(indicator_dict[indicator_of_interest]):
            year=first_year
            
            for year_value in indicator_dict[indicator_of_interest][country]:
                if year_value != '':
                    if float(year_value) > max_value_search:
                        countries_for_max_value_per_year = {}
                        max_value_search=float(year_value)
                        countries_for_max_value_per_year.update({year:[country]})
                    elif float(year_value) == max_value_search:
                        if year in countries_for_max_value_per_year:
                            countries_for_max_value_per_year[year].append(country)
                        else:
                            countries_for_max_value_per_year.update({year:[country]})
                year+=1
        if max_value_search%int(max_value_search) == 0:
            max_value=int(max_value_search)
        else:
            max_value=max_value_search
    except:
        pass
    
            
if max_value == None:
    print('Sorry, either the indicator of interest does not exist or it has no data.')
else:
    print('The maximum value is:', max_value)
    print('It was reached in these years, for these countries or categories:')
    for year in sorted(countries_for_max_value_per_year):
        print('    {}: {}'.format(year, countries_for_max_value_per_year[year]))
