Mac Book AirでSTM32CubeIDEを用いて、自分のGitHubからクローンしたら、ビルドが上手くいかない。色んな対応をしても、その時は収まるけど次にまた始まるで、使えないと判断した。

それで、VSCodeでクローンして、やろうとしたら色々複雑でわからない。調べていくと色んなExtensionを使わずに、tasks.jsonとarm-none-eabi-gccで対応できることが分かったので、それを書いていく。ここで使用したのは、クローンを動かしたときのもの。

先ずは、「簡単なLEDの点灯をプロの様な形で、ベアメタルプログラミングで作ってみて、更に色々やってきた事を組み込んでいこうと思う。」の記事を読んでください。

実際に動かす時は、Startupファイルとの兼ね合いから、mainファイルに追加。

void _init(void) {  
 //  空っぽ  
}

**必要なもの**

*   **VSCode**
*   **ARM GCC Toolchain（arm-none-eabi-gcc）**
*   **STM32\_Programmer\_CLI**

新規に作るVSCodeのプロジェクトに、以前作ったSTM32CubeIDEの同じボードのプロジェクトからコピーして使うもの。基本的に同じボードでは使い回しができると思う。

*   **Startupフォルダと内部のスタートアップアセンブリプログラム**
*   **chip\_headersフォルダ**
*   **Src内にある、syscalls.cとsysmem.c**
*   **リンカファイル**
    *   **STM32F411RETX\_FLASH.ld**
    *   **STM32F411RETX\_RAM.ld**

VSCodeの場合は、buildフォルダ内に　.elfや.bin等色々と作成されるようなので、プロジェクト内にbuildフォルダを作る必要がある。

これらの手順とインクルードパス関連をtasks.jsonに書き込んでおけば、ビルド、BIN作成、マイコンへの書き込みが自動化できる。

**tasks.jsonに関して、ファイル上に記載されていない参考事項を若干書いておく。**

**tasks.jsonは、arm-none-eabi-gcc処理と、arm-none-eabi-objcopy処理、STM32\_Programmer\_CLI処理を一括にまとめて置き、それをVSCodeで　`Ctrl + Shift + B`　すれば実行できるようにするもの。**

*   versionはVSCodeで自動で2.0.0となっている。
*   tasksは作業、labelにある番号がtasksの番号、typeは何処の命令、commandは命令、argsは、引数内容。${workspaceFolder}は、プロジェクトフォルダパス
*   mkdir -p ${workspaceFolder}/build　の意味 
*   problemMatcherは、コンパイラが出したエラーや警告を、VSCodeのProblems画面に解析させる設定。\[　\]は何もしない。

*   "-mcpu=cortex-m4"　：CPUはCortex-M4  
    "-mthumb"　：ARMのThumb命令セットを使用  
    "-mfpu=fpv4-sp-d16"　：STM32F411の浮動小数点演算ユニット（FPU）の種類  
    "-mfloat-abi=hard"　：浮動小数点演算でハードウェアFPUを使う
*   "${workspaceFolder}/Startup/startup\_stm32f411retx.s"　アセンブリ言語で  
    Vector Table、Reset\_Handler、Stack pointer、割り込みベクタ等を設定している。

*   "-T",  
    "${workspaceFolder}/STM32F411RETX\_FLASH.ld"　リンカは各プログラムのメモリ上の配置をきめるもので、リンカスクリプトのターゲットはこれだという意味。

*   "-o",
*   "${workspaceFolder}/build/output.elf"　アウトプットはこのファイルにお願い根の意味。
*   .elf　はデバッグ情報なども含んだ、開発用の実行ファイルで、機械語、アドレス情報、シンボル、デバック情報がある。

　　 "group": {

　　 "kind": "build"

         }　これは、VSCodeのBuild系タスクですよという指定。

         "dependsOn":[

         "0. Create Build Directory"

         ]　これは、先にTask0を実行しろと言う意味。

**task2に出てくる**

**"isDefault": true　はBuild Taskのデフォルトにしていするという意味。Ctrl + Shift + B　がこれを実行する。**

"arm-none-eabi-objcopy"　は、ELFなどのファイル形式を別の形式に変換する

*   "-O",
*   "binary",
*   "${workspaceFolder}/build/output.elf",
*   "${workspaceFolder}/build/output.bin"
*   出力形式はバイナリーで、入力ファイルが.elf、出力ファイルが.binの指定。

"STM32_Programmer_CLI"　STM32に書き込む

*   "-c",
*   "port=SWD"　
*   -cはコマンド、SWDはSerial Wired Debug　ST-Linkの線

*   "-w",
*   "${workspaceFolder}/build/output.bin"
*   -wは書き込み、このファイルを書き込み
*   "0x08000000",
    
    "-v",
    
    "-rst"
    
    書き込み番地のしてい、Verifyの指定、リセットの指示。
    
    一連の処理は以下の指定
    
    arm-none-eabi-gcc \  
        -mcpu=cortex-m4 \  
        -mthumb \  
        -O0 \  
        -g3 \  
        -DSTM32F411xE \  
        -I./BSP \  
        -I./System \  
        -I./Driver/GPIO \  
        -I./Driver/UART \  
        Src/\*.c \  
        BSP/\*.c \  
        System/\*.c \  
        Driver/GPIO/\*.c \  
        Driver/UART/\*.c \  
        Startup/startup\_stm32f411retx.s \  
        -T STM32F411RETX\_FLASH.ld \  
        -o build/output.elf
    
    arm-none-eabi-objcopy   
        -O binary \   
        build/output.elf \  
        build/output.bin
    
*   これをSTM32に書き込んでいる。
