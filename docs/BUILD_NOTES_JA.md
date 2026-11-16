# ビルドに関する注意

このプロジェクトはPS3 GameOS向けHomebrewを想定しています。PS3向けのPPU/SPUツールチェーンとPSL1GHT環境が必要です。

参考リポジトリ:
- https://github.com/ps3dev/ps3dev
- https://github.com/ps3dev/PSL1GHT

この段階ではPSL1GHTの版やサンプル構成を固定していないため、MakefileやPKG生成手順は確定していません。推測でビルド規則を書くと動かない可能性があるため、動作確認できる環境を選んでから合わせます。

ブラウザのSpeechSynthesisはPS3上ではそのまま使えません。PS3版では利用許諾のある音声ファイルを同梱し、PS3側の音声再生処理につなぐ必要があります。

## 未完了項目

- PS3映像初期化・描画
- DualShock 3入力
- 音声再生バックエンドと音声素材
- PARAM.SFO / ICON0.PNG等
- 正式なMakefileとPKG生成
- CFW実機テスト
