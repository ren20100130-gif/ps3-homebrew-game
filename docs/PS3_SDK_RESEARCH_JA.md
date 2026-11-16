# PS3 SDK調査メモ

調査日: 2026-11-17

## 採用候補

- SDK: [ps3dev/PSL1GHT](https://github.com/ps3dev/PSL1GHT)
- 開発環境: [ps3dev/ps3dev](https://github.com/ps3dev/ps3dev)
- コンパイラ等: [ps3dev/ps3toolchain](https://github.com/ps3dev/ps3toolchain)

PSL1GHTのREADMEでは、通常のアプリは各サンプルのMakefileからビルドし、一般的な出力はELF/SELF、PKG対応ターゲットを持つサンプルでは `make pkg` を使うと説明されている。PKGのメタデータはアプリ側Makefileで設定する。

## 実装方針

1. まず最小のPS3アプリとして映像出力と終了処理を組み込む。
2. DualShock 3の入力を読み、○・×・□・△・STARTをゲームロジックへ渡す。
3. 画面には操作名とコンボ数を表示する。
4. 音声はPS3で再生可能な同梱音声を用意し、対応する音声APIに接続する。
5. SDKサンプルと同じ方式でSELF/PKGを生成する。

## 注意点

- 現時点でこのリポジトリにはPS3用の描画・入力・音声処理はまだ実装されていない。
- SDKのAPI名やMakefile変数を推測して書かない。実際のPSL1GHTサンプルと使用するSDK版に合わせる。
- GitHub上のホスト側CテストはPS3向けビルドや実機動作の代わりにはならない。
- PKGが生成されても、CFW実機での起動確認は別途必要。

## 参照

- PSL1GHT README: https://github.com/ps3dev/PSL1GHT
- PS3開発環境: https://github.com/ps3dev/ps3dev
- PS3 toolchain: https://github.com/ps3dev/ps3toolchain
