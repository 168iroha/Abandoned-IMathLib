# expression_template
　`expression_template.hpp`をインクルードすることにより`expression_template`の全てを利用をすることができます。
　`expression_template backup1`は`expression_template`の一番古いバックアップで、expression_templateで二項演算を
$$
a+b+c+d \ \Rightarrow\  +(+(+(a,b),c),d)
$$
のようにして評価するものです。`expression_template backup2`はこの評価方法を変えて
$$
a+b+c+d \ \Rightarrow\  +(a,b,c,d)
$$
のようにしたものです。`expression_template`は`expression_template backup2`に対して畳み込み式を用いて改良しようとしたものです。

# メモについて
　`memo1.md`と`memo2.md`は`expression_template backup1`の作成の際に記述したメモです。`memo3.md`は`expression_template backup2`の作成の際に記述したメモで書きかけです。ただし、これらはQiitaに投稿していた文章そのままであり、校正していません。そのうち校正(特にQiitaのMarkdownとの仕様の差異やURLに関して)する可能性はあります。

