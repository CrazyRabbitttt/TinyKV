### 日志的数据存储格式
> 日志按照 Block 进行数据的存储，每个 Block 是固定的32kb大小
> 当然为了避免空间的浪费，如果说一条 Record 放在一个 block 内放不开的话可以存放部分到本 block中
> 剩余的部分放入到另一个 block 内，头部使用 `7byte` tag进行标注。也就是如果说本 block 剩余的空间
> 不足以放置整个 Record， 那么会自动进行拆分存放到不同的Block中

- `Full Record` : 一个 **Record** 完整的放在了一个 Block
- `First`
- `Middle`
- `Last`

**Header format**
```shell
Crc  Len   Type
4     2     1    ====>  (7bytes in total)        
```
> 如果说block剩余的空间不足以填充 `头部字段`，剩余的空间用 `0x00` 进行填充，头部字段
> 直接写到下一个block的开头位置

### Writable File
> 写文件的抽象，在 `util/writable_file.xx`下， 使用 fstream 进行的文件的写入，内嵌了
> 一个 buffer，`Flush` 调用后会将内存缓冲区的内容刷新到OS的缓冲区。想要直接落盘需要调用
> `Sync`：底层也就是调用了 `fsync` 系统调用




