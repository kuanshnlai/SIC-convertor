# SIC-convertor

# System_program 簡易組譯器期末project
## 開發環境
windows 10 code block

## 自定義變數及資料結構
* Table : 採用 c++ map 來實作，用來支援symbol Table 及 OPTable
支援的操作:
1. find  : 尋找key是否存在，若不存在則return -1
2. print : 印出整個table

* Line  : 以行為單位將讀到的輸入暫存在此類別中，並且machine_code 及 address也存在此結構中
支援的操作:
1. parsing : 將輸入字串分割成 label instruction operand 並暫存
2. set_* 系列:設定machine_code的某一field，如set_op為設定machine_code的op field
3. overloading << 支援顯示該行資訊

## 程式運作流程

### read instruction from input file and parsing instruction
流程:
step 0.從input file讀入instruction
step 1.利用" "or"\t"來分割字串
step 2.分析token是instruction 或是 label 或是operand
step 2.a 若OPTable或是AssemblyDirected Table找的到token,或是token第一個字元是'+'(type 4)=>代表token是instruction
step 2.b 若該line還未出現instruction則此token為label
step 2.c 非2.a或2.b則token為operand
step 3. 重複step 0 ~ step 2直到讀到檔案結尾
### Pass1 流程(參考課本p 53)
流程:
輸入 : 剖析完畢的Line結構(此時instruction operand label 已正確被分割出來)
step 0. 查看第一個指令是否為START
step 0.a 若是，則START的operand為起始位置及locctr(hex)
step 0.b 若否，則預設起始位置為0
step 1. 查看當前Line是否有label欄位，且assign locctr給此Line
step 1.a 若有，把label當作key，目前locctr當作value加入symbol table內
step 1.b 若無，則不動作
step 2. 查看當前Line的instruction欄位為何種type，或者instruction是Assembly Directive
step 2.a type 2(使用窮舉方式比對instruction 是否是TIXR、CLEAR、COMPR)，則locctr+=2，且設定此Line的長度為2
step 2.b type 3，locctr+=3，且設定此Line的長度為3
step 2.c type 4，locctr+=4，且設定此Line的長度為4
step 2.d Assembly Directive，locctr只有遇到BYTE、WORD、RESB、RESW才需變動
step 3. 重複step 1 ~ step 2直到遇到END或是處理完所有指令
step 4. 計算檔案長度

### Pass2 流程(參考課本 p 54)
流程:
輸入 : 剖析完畢的Line結構(此時instruction operand label 已正確被分割出來，且Symbol table已建好，且指令都已經assign address)

step 0. 把header record寫入output file
step 1. 讀取當前Line，並且判斷該Line屬於何type的指令
step 1.a type 2，若為CLEAR或TIXR則只有一個暫存器只要寫入r1 field即可，若為COMPR則需寫入r1、r2 field
step 1.b.1 type 3，判斷addressing mode
* step 1.b.1.i immediate addressing mode ，set i field
* step 1.b.1.ii indirect addressing mode ，set n field
* step 1.b.1.iii direct addressing mode， set i and n field

step 1.b.2 判斷是否可用pc relative或base relative完成
* step 1.b.2.i 若可用pc relative，set p field 和 disp field
* step 1.b.2.ii 若pc無法但可用base relative 則set b field和disp field(註:BASE XXX要在此指令前先設定，否則無法用base relative)
* step 1.b.2.iii 上述兩情況皆無法則error

step 1.c type 4，set e field和disp field，target address可以直接放絕對位置，且先將此指令寫入Modfication record，並且暫置於串流中(不直接寫入output file)
step 1.d Assembly Directive 特殊處理
step 2 寫入Text record，(若Text record寫滿則開新Text record)
step 3 重複step 1、2直到遇到END
step 4 寫入 modification record
step 5 寫入 end record

## 心得
這次的作業我覺得蠻有趣的，但因為這學期有其他的外務必須處理，因此無法花太多的時間來做，從開始做到結束大概花了兩天左右，在實作過程中遇到的困難主要是某些指令需要用特殊情況去處理，例如某些指令的operand並不是單純的label或者constant，而是後面還加上X暫存器，這就需要將operand再進行分割，或者是BYTE後面接的operand長度可以不一致(1或2或3)，
所以我的選擇是先將程式架構完成，再去慢慢處理這些特例，完成這次作業讓我對剖析、組譯程式產生興趣，不過SIC/XE程式的輸入已經算是固定了(每個指令可以分成 label instruction operand)，因此可以輕易地觀察到輸入指令的規則，用簡單的字串分割就可以剖析整個指令，像其他類高階程式語言剖析起來一定更加不易，因此我打算下學期再修黃老師的compiler設計去了解整個compiler設計到底是如何實作，感謝黃老師及助教這學期的教學

## COPYRIGHT CLAIM
此程式實作皆由學生賴冠勳本人完成
