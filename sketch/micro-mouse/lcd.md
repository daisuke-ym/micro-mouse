# AQM1602Y に表示する内容

```
  0123456789ABCDEF
0 XX YY  AAAA BBBB
1 SSSSS  CCCC DDDD
```

|シンボル|意味
|-----| --
|XX   | マウスのX座標
|YY   | マウスのY座標
|AAAA | FLセンサの値
|BBBB | FRセンサの値
|CCCC | Lセンサの値
|DDDD | Rセンサの値
|SSSSS| マウスの状態を示すフラグ(ORSCA)

|フラグ|意味
|---|--
|O|往路走行中
|R|復路走行中
|S|最短経路走行中
|C|迷路全探索完了
|A|マウスを通路中央に値調整中

