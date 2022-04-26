#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import time
import subprocess
import psutil


def get_memory() -> dict:
    """
    Obtains memory values from the system.

    Average execution time after 10.000 runs with time.time() is
    3.42e-05 s.

    :return: dict, memory values
    """
    memory = psutil.virtual_memory()
    data = dict()

    data["memory_used"] = memory.used
    data["memory_free"] = memory.free
    data["memory_available"] = memory.available

    return data


def get_swap() -> dict:
    """
    Obtains swap memory values from the system.

    Average execution time after 10.000 runs with time.time() is
    7.93e-05 s.

    :return: dict, swap memory values
    """
    swap = psutil.swap_memory()
    data = dict()

    data["swap_used"] = swap.used
    data["swap_free"] = swap.free

    return data


def get_cpu() -> dict:
    """
    Obtains CPU values from the system.

    Average execution time after 10.000 runs with time.time() is
    2.13e-05 s.

    :return: dict, CPU values
    """
    cpu = psutil.cpu_stats()
    data = dict()

    # d["cpus_physical"] = cpu.cpu_count(logical=False)
    # d["cpus_logical"] = cpu.cpu_count
    data["ctx_switches"] = cpu.ctx_switches
    data["interrupts"] = cpu.interrupts
    data["cpu_soft_interrupts"] = cpu.soft_interrupts

    return data


def get_cpu_load(percpu: bool,
                 percent: bool) -> dict:
    """
    Obtains CPU load values from the system, either as a percentage
    or as absolute values. The CPU output in percent is between 0 and
    10. Therefore, it is multiplied by 10 to reach 0 to 100.

    Average execution time after 10.000 runs with time.time() is
    4.28e-05 s. Execution psutil.cpu_times_percent takes much longer,
    even with the lowest recommended value for the argument
    interval=0.1.

    :param percpu: bool, | measurements for all CPUs together or
                         | individually
    :param percent: bool, load as percentage or absolute values
    :return: dict, CPU load values
    """
    if percent:
        cpu_load = psutil.cpu_times_percent(interval=0.1,
                                            percpu=percpu)
        f = 10
    else:
        cpu_load = psutil.cpu_times(percpu=percpu)
        f = 1

    data = dict()
    if percpu:
        for i, cpu in enumerate(cpu_load):
            if i > 3:
                break

            data["cpu" + str(i) + "_user"] = cpu.user * f
            data["cpu" + str(i) + "_nice"] = cpu.nice * f
            data["cpu" + str(i) + "_system"] = cpu.system * f
            data["cpu" + str(i) + "_idle"] = cpu.idle * f
            data["cpu" + str(i) + "_iowait"] = cpu.iowait * f
            data["cpu" + str(i) + "_irq"] = cpu.irq * f
            data["cpu" + str(i) + "_softirq"] = cpu.softirq * f
            data["cpu" + str(i) + "_steal"] = cpu.steal * f
    else:
        data["cpu_user"] = cpu_load.user * f
        data["cpu_nice"] = cpu_load.nice * f
        data["cpu_system"] = cpu_load.system * f
        data["cpu_idle"] = cpu_load.idle * f
        data["cpu_iowait"] = cpu_load.iowait * f
        data["cpu_irq"] = cpu_load.irq * f
        data["cpu_softirq"] = cpu_load.softirq * f
        data["cpu_steal"] = cpu_load.steal * f

    return data


def get_cpu_freq(percpu: bool) -> dict:
    """
    Obtains CPU frequency per CPU or for the whole system.

    :param percpu: bool, | measurements for all CPUs together or
                         | individually
    :return: dict, CPU frequency per threat
    """
    cpu_freq = psutil.cpu_freq(percpu=percpu)
    data = dict()

    if percpu:
        for i, cpu in enumerate(cpu_freq):
            data["cpu" + str(i) + "_freq"] = cpu.current
    else:
        data["cpu_freq"] = cpu_freq.current

    return data


def get_temperature() -> dict:
    """
    Obtains temperature values from the system.

    Average execution time after 10.000 runs with time.time() is
    0.016 s.

    :return: dict, temperature values
    """
    temps = psutil.sensors_temperatures()
    data = dict()

    try:
        data["cpu_temp"] = temps["cpu_thermal"][0][1]
    except KeyError:
        data["cpu_temp"] = -1
        # print("cpu_thermal is most likely just accessible "
        #       "via a Raspberry Pi.")

    return data


def get_fan() -> dict:
    """
    Obtains fan values from the system.

    Average execution time after xx runs with time.time() is
    xx s.

    :return: dict, fan values
    """
    pass
    # fans = psutil.sensors_fans()
    # data = dict()
    #
    # return data


def get_disk() -> dict:
    """
    Obtains disk/hard drive values (storage and I/O)from the system.

    Average execution time after 10.000 runs with time.time() is
    28.80e-05 s.

    :return: dict, disk/hard drive values
    """
    disk = psutil.disk_usage("/")
    data = dict()

    data["disk_total"] = disk.total
    data["disk_free"] = disk.free
    data["disk_used"] = disk.used

    disk_io = psutil.disk_io_counters()
    data["disk_read_bytes"] = disk_io.read_bytes
    data["disk_read_count"] = disk_io.read_count
    data["disk_read_merged_count"] = disk_io.read_merged_count
    data["disk_read_time"] = disk_io.read_time
    data["disk_write_bytes"] = disk_io.write_bytes
    data["disk_write_count"] = disk_io.write_count
    data["disk_write_merged_count"] = disk_io.write_merged_count
    data["disk_write_time"] = disk_io.write_time

    return data


def get_network() -> dict:
    """
    Obtains network values from the system.

    Average execution time after 10.000 runs with time.time() is
    4.31e-05 s.

    :return: dict, network values
    """
    network = psutil.net_io_counters()
    data = dict()

    data["bytes_recv"] = network.bytes_recv
    data["bytes_sent"] = network.bytes_sent
    data["dropin"] = network.dropin
    data["dropout"] = network.dropout
    data["errin"] = network.errin
    data["errout"] = network.errout
    data["packets_recv"] = network.packets_recv
    data["packets_sent"] = network.packets_sent

    return data


def get_uptime() -> dict:
    """
    Obtains uptime in seconds from the system. Psutil returns the
    seconds since 1970. Therefore, the difference between that and
    the current elapsed seconds is calculated for up time.

    Average execution time after 10.000 runs with time.time() is
    1.54e-05 s.

    :return: dict, uptime values
    """
    # ToDo: Reduce the frequency as 1ms is to much for this parameter.
    up_time = psutil.boot_time()
    data = dict()

    up_time = int(time.time()) - int(up_time)
    data["up_time"] = up_time

    return data


def get_entropy() -> int:
    """
    Read the currently available entropy in bits on the system for
    cryptographic methods. For reference: A very strong private key
    typically contains 256 bits of entropy.

    :return: int, available entropy in bits
    """
    entropy = subprocess.check_output(
        ["cat", "/proc/sys/kernel/random/entropy_avail"]).decode()

    return int(entropy)


# Functions to get and process pressure stall information from
# Christoph Doblander, TUM Informatics Chair I13.
# ----------------------------------------------------------------------
def file_exists(full_path: str) -> bool:
    """
    Checks if a file in a given directory or a path exists.

    :param full_path: string, | full path of folder where file is stored
                              | with suffix e.g. .csv or directory
    :return: boolean, file or directory exist or not
    """
    return os.path.isfile(full_path)


def read_pressure_stall_from_proc(path: str) -> str:
    """
    Reads info from the /proc/pressure directory. It contains one file
    for cpu, io and memory. Only the line with "some" at the beginning
    is used.

    :param path: str, path/directory to pressure file
    """

    if file_exists(path):
        with open(path, 'r') as proc:
            line = proc.readline()
            if line.startswith('some'):
                return line
    else:
        return ""


def pressure_stall_as_dictionary(prefix: str,
                                 line: str) -> dict:
    """
    Splits the "some" line of the cpu, io and memory file from
    /proc/pressure directory at the equal sign and generate a dict out
    of the resulting variables.

    :param prefix: str, either cpu, io or memory
    :param line: str, read input from /proc/pressure/<prefix> file
    :return: dict, | Average and total values of cpu, io and
                   | memory pressure
    """
    data = dict()
    try:
        parts = line.split('some ')[1].split(' ')

        for part in parts:
            k, v = part.split('=')
            if k == "total":
                data["psi_" + prefix + "_" + k] = int(v)
            else:
                data["psi_" + prefix + "_" + k] = float(v)
        return data

    except IndexError:
        return data


def read_psi(prefix: str) -> dict:
    """
    Reads one file in /proc/pressure and processes the output to a
    dictionary.

    :param prefix: str, either cpu, io or memory
    :return: dict, all variables from the cpu, io and memory file
    """
    return pressure_stall_as_dictionary(prefix,
                                        read_pressure_stall_from_proc(
                                            '/proc/pressure/' + prefix))


def get_pressure_stall() -> dict:
    """
    Combines the processed output of cpu, io and memory file into one
    dictionary.

    Average execution time after 10.000 runs with time.time() is
    5.70e-05 s.

    :return: dict, data from cpu, io and memory file
    """
    try:
        stall = {**read_psi('cpu'),
                 **read_psi('io'),
                 **read_psi('memory')}

        return stall

    except KeyError:
        return {}
# ----------------------------------------------------------------------


# Get temperature of connected drives which are SMART ready.
# ----------------------------------------------------------------------
def get_device_name() -> list:
    """
    Returns the name of all connected drives which are SMART compatible.

    :return: list, name of connected drives
    """
    stdout = os.popen("smartctl --scan")
    data = stdout.read()
    scan = data.split(" ")

    drives = []
    for s in scan:
        if "/dev/" in s and len(s) < 9:
            drives.append(s.replace(",", ""))

    return list(set(drives))


def get_disk_temp(drives: list) -> dict:
    """
    Stores the device name with its respective temperature into a
    dictionary.

    :param drives: list, names of connected drives
    :return: dict, name of drive with its respective temperature
    """
    drive_temp = {}

    for i, drive in enumerate(drives):
        stdout = os.popen("sudo hddtemp " + drive)
        data = stdout.read().splitlines()
        data = data[0].split(" ")[-1]

        try:
            temperature = float(data.replace("°C", ""))
        except ValueError:
            temperature = -1

        drive_temp["drive_" + str(i)] = temperature

    return drive_temp
# ----------------------------------------------------------------------


# vcgencmd Raspberry Pi specific command.
# ----------------------------------------------------------------------
def get_throttle() -> int:
    """
    Transform a hexadecimal number to a binary one.

    Table describing the meaning of the output of vcgencmd command.
    Bit 	Hex value 	Meaning
    0 	        1 	    Under-voltage detected
    1 	        2 	    Arm frequency capped
    2 	        4 	    Currently throttled
    3 	        8       Soft temperature limit active
    16 	        10000 	Under-voltage has occurred
    17 	        20000 	Arm frequency capping has occurred
    18 	        40000 	Throttling has occurred
    19 	        80000 	Soft temperature limit has occurred

    :return: int, binary number
    """
    # ToDo: Why is byte_str one digit to short?
    try:
        output = subprocess.check_output(
            ["vcgencmd", "get_throttled"]).decode()
    except FileNotFoundError:
        print("If vcgencmd called from a Raspberry Pi try adding user:"
              "sudo usermod -aG video <username>")

    output = "0x50000"
    n = int(output, 16)
    byte_str = ""
    while n > 0:
        byte_str = str(n % 2) + byte_str
        n = n >> 1

    print(len(byte_str))
    print(len("01010000000000000000"))
    if byte_str == "01010000000000000000":
        print("Worked")

    return int(output.strip().split("=")[1], 16)


def eval_throttle():
    throttled_state = get_throttle()

    # ToDo : How to store throttle states and when does an encountered throttle disappeare in the bytes?
    if not throttled_state:
        return 0

    if throttled_state & ThrottledState.UNDER_VOLTAGE_ACTIVE:
        print("Under-voltage detected")
    if throttled_state & ThrottledState.ARM_FREQUENCY_CAPPING_ACTIVE:
        print("Arm frequency capped")
    if throttled_state & ThrottledState.THROTTLING_ACTIVE:
        print("Currently throttled")
    if throttled_state & ThrottledState.SOFT_TEMPERATURE_LIMIT_ACTIVE:
        print("Soft temperature limit active")
    if throttled_state & ThrottledState.UNDER_VOLTAGE_HAS_OCCURRED:
        print("Under-voltage has occurred")
    if throttled_state & ThrottledState.ARM_FREQUENCY_CAPPING_HAS_OCCURRED:
        print("Arm frequency capping has occurred")
    if throttled_state & ThrottledState.THROTTLING_HAS_OCCURRED:
        print("Throttling has occurred")
    if throttled_state & ThrottledState.SOFT_TEMPERATURE_LIMIT_HAS_OCCURRED:
        print("Soft temperature limit has occurred")

