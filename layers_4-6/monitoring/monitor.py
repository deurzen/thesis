#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from time import sleep
from datetime import datetime

import psycopg2
import argparse

from metrics import *

parser = argparse.ArgumentParser(description='node monitoring script')
parser.add_argument('-n', '--host', type=str, default='127.0.0.1', help='hostname of the database service')
parser.add_argument('-p', '--port', type=int, default=5432, help='port behind which the database is listening')
parser.add_argument('-U', '--user', type=str, required=False, default='postgres', help='database user')
parser.add_argument('-P', '--password', type=str, required=True, help='database password')
parser.add_argument('-t', '--table', type=str, required=True, help='name of database table to insert data into')
parser.add_argument('-d', '--duration', type=int, required=False, help='how long to continue running (in seconds, default: indefinitely)')
parser.add_argument('-i', '--interval', type=float, required=False, default=1, help='how frequently to poll for data and insert into database (in seconds)')
args = parser.parse_args()

def indefinitely():
    while True:
        yield

def main():
    """
    Monitors sensors and parameters from a Linux machine with mainly
    psutil (https://github.com/giampaolo/psutil).
    """

    conn = None
    cursor = None

    try:
        conn = psycopg2.connect(user=args.user,
                password=args.password,
                host=args.host,
                port=args.port)

        cursor = conn.cursor()
        duration = range(args.duration) if args.duration else indefinitely()

        for _ in duration:
            memory = get_memory()
            swap = get_swap()
            cpu = get_cpu()
            cpu_load = get_cpu_load(percpu=True, percent=True)
            cpu_freq = get_cpu_freq(percpu=True)
            cpu_temp = get_temperature()
            disk = get_disk()
            network = get_network()
            entropy = get_entropy()
            up_time = get_uptime()
            pressure_stall = get_pressure_stall()

            monitoring_data = {
                'time': str(datetime.utcnow()),
                **memory,
                **swap,
                **cpu,
                **cpu_load,
                **cpu_freq,
                **cpu_temp,
                **disk,
                **network,
                'entropy': entropy,
                **up_time,
                **pressure_stall,
            }

            # {'psi_cpu_avg10': 24.18, 'psi_cpu_avg60': 26.22, 'psi_cpu_avg300': 23.26, 'psi_cpu_total': 272216609, 'psi_io_avg10': 0.0, 'psi_io_avg60': 0.14, 'psi_io_avg300': 2.05, 'psi_io_total': 154722139, 'psi_memory_avg10': 0.0, 'psi_memory_avg60': 0.0, 'psi_memory_avg300': 0.0, 'psi_memory_total': 0}

            insert_query = f"""INSERT INTO {args.table}
            (
                time,
                memory_used, memory_free, memory_available,
                swap_used, swap_free,
                ctx_switches, interrupts, cpu_soft_interrupts,
                cpu0_user, cpu0_nice, cpu0_system, cpu0_idle, cpu0_iowait, cpu0_irq, cpu0_softirq, cpu0_steal,
                cpu1_user, cpu1_nice, cpu1_system, cpu1_idle, cpu1_iowait, cpu1_irq, cpu1_softirq, cpu1_steal,
                cpu2_user, cpu2_nice, cpu2_system, cpu2_idle, cpu2_iowait, cpu2_irq, cpu2_softirq, cpu2_steal,
                cpu3_user, cpu3_nice, cpu3_system, cpu3_idle, cpu3_iowait, cpu3_irq, cpu3_softirq, cpu3_steal,
                cpu0_freq,
                cpu_temp,
                disk_total, disk_free, disk_used,
                disk_read_bytes, disk_read_count, disk_read_merged_count, disk_read_time,
                disk_write_bytes, disk_write_count, disk_write_merged_count, disk_write_time,
                bytes_recv, bytes_sent, dropin, dropout, errin, errout, packets_recv, packets_sent,
                entropy,
                up_time,
                psi_cpu_avg10, psi_cpu_avg60, psi_cpu_avg300, psi_cpu_total,
                psi_io_avg10, psi_io_avg60, psi_io_avg300, psi_io_total,
                psi_memory_avg10, psi_memory_avg60, psi_memory_avg300, psi_memory_total
            )
            VALUES
            (
                '{monitoring_data['time']}',
                {monitoring_data['memory_used']}, {monitoring_data['memory_free']}, {monitoring_data['memory_available']},
                {monitoring_data['swap_used']}, {monitoring_data['swap_free']},
                {monitoring_data['ctx_switches']}, {monitoring_data['interrupts']}, {monitoring_data['cpu_soft_interrupts']},
                {monitoring_data['cpu0_user']}, {monitoring_data['cpu0_nice']}, {monitoring_data['cpu0_system']}, {monitoring_data['cpu0_idle']}, {monitoring_data['cpu0_iowait']}, {monitoring_data['cpu0_irq']}, {monitoring_data['cpu0_softirq']}, {monitoring_data['cpu0_steal']},
                {monitoring_data['cpu1_user']}, {monitoring_data['cpu1_nice']}, {monitoring_data['cpu1_system']}, {monitoring_data['cpu1_idle']}, {monitoring_data['cpu1_iowait']}, {monitoring_data['cpu1_irq']}, {monitoring_data['cpu1_softirq']}, {monitoring_data['cpu1_steal']},
                {monitoring_data['cpu2_user']}, {monitoring_data['cpu2_nice']}, {monitoring_data['cpu2_system']}, {monitoring_data['cpu2_idle']}, {monitoring_data['cpu2_iowait']}, {monitoring_data['cpu2_irq']}, {monitoring_data['cpu2_softirq']}, {monitoring_data['cpu2_steal']},
                {monitoring_data['cpu3_user']}, {monitoring_data['cpu3_nice']}, {monitoring_data['cpu3_system']}, {monitoring_data['cpu3_idle']}, {monitoring_data['cpu3_iowait']}, {monitoring_data['cpu3_irq']}, {monitoring_data['cpu3_softirq']}, {monitoring_data['cpu3_steal']},
                {monitoring_data['cpu0_freq']},
                {monitoring_data['cpu_temp']},
                {monitoring_data['disk_total']}, {monitoring_data['disk_free']}, {monitoring_data['disk_used']},
                {monitoring_data['disk_read_bytes']}, {monitoring_data['disk_read_count']}, {monitoring_data['disk_read_merged_count']}, {monitoring_data['disk_read_time']},
                {monitoring_data['disk_write_bytes']}, {monitoring_data['disk_write_count']}, {monitoring_data['disk_write_merged_count']}, {monitoring_data['disk_write_time']},
                {monitoring_data['bytes_recv']}, {monitoring_data['bytes_sent']}, {monitoring_data['dropin']}, {monitoring_data['dropout']}, {monitoring_data['errin']}, {monitoring_data['errout']}, {monitoring_data['packets_recv']}, {monitoring_data['packets_sent']},
                {monitoring_data['entropy']},
                {monitoring_data['up_time']},
                {monitoring_data['psi_cpu_avg10']}, {monitoring_data['psi_cpu_avg60']}, {monitoring_data['psi_cpu_avg300']}, {monitoring_data['psi_cpu_total']},
                {monitoring_data['psi_io_avg10']}, {monitoring_data['psi_io_avg60']}, {monitoring_data['psi_io_avg300']}, {monitoring_data['psi_io_total']},
                {monitoring_data['psi_memory_avg10']}, {monitoring_data['psi_memory_avg60']}, {monitoring_data['psi_memory_avg300']}, {monitoring_data['psi_memory_total']}
            )"""

            cursor.execute(insert_query)
            conn.commit()

            sleep(args.interval)

    except (Exception, psycopg2.Error) as error:
        print(error)
        return
    finally:
        if cursor is not None:
            cursor.close()
        if conn is not None:
            conn.close()


if __name__ == "__main__":
    main()
