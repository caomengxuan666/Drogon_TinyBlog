#!/bin/bash
# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# 跳转到脚本所在目录的上一级目录
cd "$SCRIPT_DIR/.."
pip install -r requirements.txt