# Counselling-System
The Counselling System allows candidates to provide a list of preferences, specifying their desired institutes and branches. The admission process considers candidates' ranks in the qualifying exams and allocates seats based on preferences and seat availability in each branch.

# Engineering Counselling and Admission System

## Table of Contents
1. [Introduction](#introduction)
2. [Project Description](#project-description)
3. [Files ](#files)
4. [Dependencies](#dependencies)
5. [Usage](#usage)


## Introduction

This software system is designed to streamline the engineering counselling and admission process for two sets of institutes - Indian Institutes of Technology (IITs) and National Institutes of Technology (NITs). The system aims to efficiently allocate seats based on candidates' preferences and their ranks in the respective qualifying exams, JEE-Advanced and JEE-Main.

## Project Description

The Engineering Counselling and Admission System is developed to tackle the challenges of managing a large number of candidates and available seats across different branches in each institute. The system allows candidates to provide a list of preferences, specifying their desired institutes and branches. The admission process considers candidates' ranks in the qualifying exams and allocates seats based on preferences and seat availability in each branch.

## Files

The project files are organized as follows:

Updated Files:


  - `allotments.txt`: This file contains the final allotment of seats to candidates after the counselling process.
  - `seat_matrix.txt`: This file provides the seat matrix for different branches and institutes, indicating the number of available seats in each branch.
  - `student_info.csv`: This file contains information about the candidates, including their ranks in JEE-Advanced and JEE-Main, as well as their preferences for institutes and branches.
  - `files.txt`: Consists of File paths to update rank list or student info files.

## Dependencies

The project is implemented in C and requires a gcc C compiler to build and run. No additional external libraries are used. This project is optimised to run on Linux and MacOS Environment.

## Usage

1. Ensure you have a gcc C compiler installed on your system.
2. Clone this repository to your local machine.
3. Navigate to the project directory, ensure that current directory has main.c
4. Compile the source code using the following command:

```bash
gcc main.c && ./a.out
```

5. Follow the on-screen instructions to provide candidate preferences and view the allocated seats.



