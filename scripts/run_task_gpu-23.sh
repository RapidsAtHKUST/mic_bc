#!/usr/bin/env bash

export KMP_AFFINITY=granularity=fine,compact
export MIC_ENV_PREFIX=PHI
export PHI_KMP_AFFINITY=granularity=fine,compact



data_dir="../../graphs/"

graphs=( "delaunay_n17" "smallworld" "email-Enron" "" )

vertices_ratio=( "20%" "40%" "60%"  "80%"  "100%" )