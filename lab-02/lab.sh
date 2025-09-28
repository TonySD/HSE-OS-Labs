#!/bin/bash
: '
Написать программу, осуществляющую вывод в создаваемый по запросу файл через межпроцессный канал из параллельного процесса 
полного имени текущего каталога и списка файлов текущего каталога. Предусмотреть в программе возможность неоднократного 
прерывания от клавиатуры. При поступлении первого прерывания вывести дополнительно и количество блоков, отводимых под каждый 
файл текущего каталога. При большем количестве прерываний вывести только общее количество блоков, отведенных под каталог.
'

interrupt_count=0
pipe_file="/tmp/crossprocess_fifo_pipe"

if [[ ! -p $pipe_file ]]; then
    mkfifo $pipe_file
fi

handle_interrupt() {
    # incrementing interrupt count
    interrupt_count=$(( interrupt_count + 1 ))

    if [ $interrupt_count -eq 1 ]; then
        echo -e "\nFirst interrupt, output the number of blocks for each file in the current directory:"
        # Sed for removing the last line with total blocks
        du -a | sed '$d'
    else
        echo -e "\nInterrupt signal received, total number of blocks: "
        du -s .
    fi
}

# setting interrupt handler
trap handle_interrupt SIGINT

while true; do
    echo "Please, enter the file, which will contain result (enter 'q' for exit): "
    read filename
    
    if [ "$filename" = "q" ]; then
        break
    fi
    
    # creating child process for writing to the pipe
    (
        echo -e "Current directory absolute path: "
        pwd
        echo -e "\nCurrent directory files: "
        ls -a
    ) > $pipe_file &
    
    # reading from the pipe and writing to the file
    cat $pipe_file > "$filename"
    echo "Data saved to $filename"
done

# deleting the named pipe
rm $pipe_file





