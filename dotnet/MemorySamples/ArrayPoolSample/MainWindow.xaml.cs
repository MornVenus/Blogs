using System.Buffers;
using System.Diagnostics;
using System.Windows;

namespace ArrayPoolSample;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void LargeBytesReturnBtn_Click(object sender, RoutedEventArgs e)
    {
        var pool = ArrayPool<byte>.Shared;

        var bucketsField = pool.GetType().GetField("t_tlsBuckets", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
        if (bucketsField == null) return;
        var buckets = bucketsField.GetValue(pool);

        var bytes = pool.Rent(1025 * 1024 * 1024);
        pool.Return(bytes);
    }

    private void ReturnCustomBytesBtn_Click(object sender, RoutedEventArgs e)
    {
        var pool = ArrayPool<byte>.Shared;

        var bucketsField = pool.GetType().GetField("t_tlsBuckets", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
        if (bucketsField == null) return;
        var buckets = bucketsField.GetValue(pool);

        var bytes = new byte[129];
        bytes[0] = 1;
        bytes[1] = 2;
        bytes[2] = 3;

        pool.Return(bytes);

        var buffer = pool.Rent(128);

        MessageBox.Show((bytes == buffer).ToString());

    }

    private async void ThreadLocalBtn_Click(object sender, RoutedEventArgs e)
    {
        var pool = ArrayPool<byte>.Shared;

        var bytes = new byte[128];
        bytes[0] = 1;
        bytes[1] = 2;
        bytes[2] = 3;

        pool.Return(bytes);

        await Task.Run(() =>
        {
            var pool2 = ArrayPool<byte>.Shared;
            
            var ret = pool2.Rent(128);
            Debug.WriteLine((ret == bytes).ToString());
        });
    }
}