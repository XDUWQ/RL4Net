#!/usr/bin/env python
# coding=utf-8
"""
@author: Jiawei Wu
@create time: 2020-3-10 19:56
@edit time: 2020-03-29 16:35
"""
import argparse
import json
import os
from pathlib import Path


parser = argparse.ArgumentParser(description='安装参数')
parser.add_argument('--wafdir', default='/ns-3-dev', type=str, help='安装目录')
# 其他参数
parser.add_argument('--noaddon', default=False, action='store_true', help='是否执行addon代码')
parser.add_argument('--noenv', default=False, action='store_true', help='是否执行env代码')

args = parser.parse_args()

cur_path = Path().resolve()


def exec_addon_setup():
    addon_path = cur_path / 'ns3-addon'
    os.chdir(addon_path)
    os.system(f'python addon_setup.py --wafdir={args.wafdir} --noreconf --norebuild')


def exec_env_setup():
    env_path = cur_path / 'ns3-env'
    os.chdir(env_path)
    os.system(f'python env_setup.py --wafdir={args.wafdir} ')


if __name__ == '__main__':
    if not args.noaddon:
        exec_addon_setup()
    if not args.noenv:
        exec_env_setup()