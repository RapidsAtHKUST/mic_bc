#!/usr/bin/env bash

export KMP_AFFINITY=granularity=fine,compact
export MIC_ENV_PREFIX=PHI
export PHI_KMP_AFFINITY=granularity=fine,compact


base_dir="/home/lwangay/workspace"
data_dir="${base_dir}/graphs"
bin_dir="${base_dir}/mic_bc/build"
bin_name="mic_bc"
flags="0x00ee"
threshold="0.3"

numaCTL=numactl
numaPRE="$numaCTL --membind 1"


graphs=( "delaunay_n17.graph" "smallworld.graph" "email-Enron.txt" )

cd ${bin_dir}

for i in "${graphs[@]}"
do
    echo ${numaPRE} ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "10%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "20%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "20%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "30%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "40%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "40%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "50%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "60%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "60%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "70%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "80%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "80%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "90%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "100%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "100%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'

done