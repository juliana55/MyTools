# 【Android】写真をクラウドに保存しよう！ for Kotlin

![画像1](/readme-img/OverView.png)

## 概要

* [ニフクラ mobile backend](https://mbaas.nifcloud.com/) の『ファイルストア機能』を利用して、「撮った写真をクラウドに保存する」内容を実装したサンプルプロジェクトです
* 簡単な操作ですぐに [ニフクラ mobile backend](https://mbaas.nifcloud.com/)の機能を体験いただけます

## ニフクラ mobile backendって何？？
スマートフォンアプリのバックエンド機能（プッシュ通知・データストア・会員管理・ファイルストア・SNS連携・位置情報検索・スクリプト）が**開発不要**、しかも基本**無料**(注1)で使えるクラウドサービス！今回はデータストアを体験します

注1：詳しくは[こちら](https://mbaas.nifcloud.com/price.htm)をご覧ください

![画像2](/readme-img/SdkTypes.png)

## 動作環境
* Windows 7 Professional
* Android Studio 3.1
* Android ver 4x,5x
  - このサンプルアプリは、端末のカメラを使用するため、実機が必要です
* Android SDK v3
  - SDK v2系だと動作しないので注意

※上記内容で動作確認をしています


## 手順
### 1. ニフクラ mobile backend の会員登録・ログインとアプリの新規作成
* 下記リンクから会員登録（無料）をします
  * https://console.mbaas.nifcloud.com/signup
* 登録ができたら下記リンクからログインします
  * https://console.mbaas.nifcloud.com/
* 下図のように「アプリの新規作成」画面が出るのでアプリを作成します
  * 既に mobile backend を利用したことがある方は左上の「＋新しいアプリ」をクリックすると同じ画面が表示されます

![画像3](/readme-img/mBassNewProject.png)

* アプリ作成されると下図のような画面になります
* この２種類のAPIキー（アプリケーションキーとクライアントキー）は先ほどインポートしたAndroidStudioで作成するAndroidアプリにニフクラ mobile backendの紐付けるため、あとで使います

![画像4](/readme-img/mBassAPIkey.png)

* 動作確認後に写真（画像）が保存される場所も確認しておきましょう

![画像5](/readme-img/mBassData.png)

### 2. サンプルプロジェクトのダウンロード
* 下記リンクをクリックしてプロジェクトをダウンロードします
  * https://github.com/NiFCloud-mbaas/KotlinFileApp/archive/master.zip
* ダウンロードしたプロジェクトを解凍します
* AndroidStudio を開きます、「Open an existing Android Studio projct」をクリックして解凍したプロジェクトを選択します

![画像6](/readme-img/SelectProject.png)

* プロジェクトを開きます

![画像7](/readme-img/ProjectDesign.png)

### 3. SDKの導入（実装済み）

※このサンプルアプリには既にSDKが実装済み（下記手順）となっています。（ver.3.0.0)<br>　最新版をご利用の場合は入れ替えてご利用ください。

* SDKダウンロード
SDKはここ（[SDK リリースページ](https://github.com/NIFCloud-mbaas/ncmb_android/releases)）から取得してください.
  - NCMB.jarファイルがダウンロードします。
* SDKをインポート
  - app/libsフォルダにNCMB.jarをコピーします
* 設定追加
  - app/build.gradleファイルに以下を追加します
```gradle
dependencies {
    compile 'com.google.code.gson:gson:2.3.1'
    compile files('libs/NCMB.jar')
}
```
  - androidManifestの設定
    - <application>タグの直前に以下のpermissionを追加します
```html
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
```

### 4. APIキーの設定

* AndroidStudio で MainActivity.kt を開きます
  * ディレクトリはデフォルトで「Android」が選択されていますので、「Project」に切り替えてから探してください

![画像09](/readme-img/009.png)

* APIキー（アプリケーションキーとクライアントキー）の設定をします

![画像8](/readme-img/AndroidAPIkey.png)

* それぞれ`YOUR_APP_KEY`と`YOUR_CLIENT_KEY`の部分を書き換えます
  * このとき、ダブルクォーテーション（`"`）を消さないように注意してください！

### 5. 動作確認

* エミュレーターでアプリをビルドします
  * 失敗する場合は一度「Clean Project」を実行してから再度ビルドしてください
* アプリが起動したら
  1. 「CAMERA」ボタンをタップして、写真を撮影します
  2. 「保存」ボタンをタップして、保存します
    - 今回はファイル名固定しています：`test.png`
* ニフクラ mobile backend 上に画像が保存されると、アプリの画面に画像表示されます

![画像9](/readme-img/AndroidCamera.png)

* 保存に成功したら、[ニフクラ mobile backend](https://mbaas.nifcloud.com/) の管理画面から「ファイルストア」を確認してみましょう！

![画像10](/readme-img/mBassFileStore.png)

* 簡単に写真がクラウドに保存できました

## 解説
サンプルプロジェクトに実装済みの内容のご紹介します。

* `activity_main.xml`でデザインを作成し、`MainActivity.kt`にロジックを書いています

### ファイルのアップロード

* ファイルストアを利用するには、SDKが提供する `NCMBFile` を使用します
* ファイルのアップロードを行うには、このクラスが提供する `saveInBackground` メソッドを利用します（非同期処理）
* ファイル名を固定しているため、新しくファイル（画像）を保存すると上書きされます

```Kotlin
val file = NCMBFile("test.png", dataByte, acl)
file.saveInBackground { e ->
val result: String
if (e != null) {
    //保存失敗

} else {
    //保存成功

}
```

### アップロードしたファイルを取得

* ファイルストアに保存したものを取得するには、`fetch` メソッドを利用します（同期処理）
  - `fetchInBackground` メソッドでの非同期処理も利用可能です
 - ファイルの取得時には、ファイル名を先に取得する必要があります(今回はファイル名固定：`test.png`)

```kotlin
val file = NCMBFile("test.png")
file.fetchInBackground { dataFetch, er ->
   if (er != null) {
       //失敗処理

   } else {
       //成功処理

   }
}
```

## 参考
* ニフクラ mobile backend の[ドキュメント（ファイルストア（Android））](https://mbaas.nifcloud.com/doc/current/filestore/basic_usage_android.html)（for Java）をご用意していますので、ご活用ください
