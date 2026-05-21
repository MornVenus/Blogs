using System.Net.Sockets;

namespace TcpLib;

public class TcpCommService
{
    private TcpClient? _client;

    public bool Connect(string ip, int port)
    {
        _client = new TcpClient();

        try
        {
            _client.Connect(ip, port);
            _client.SendTimeout = 10 * 60 * 1000; // 发送超时时间为 120s
            var stream = _client.GetStream();
            // 开启一个长期线程用于接收数据
            Task.Factory.StartNew(()=> ReceiveData(stream), TaskCreationOptions.LongRunning);
        }
        catch (Exception)
        {
            return false;
        }
       
        return _client.Connected;
    }

    /// <summary>
    /// 接收Tcp数据
    /// </summary>
    /// <param name="stream"></param>
    private void ReceiveData(NetworkStream stream)
    {
        byte[] buffer = new byte[10 * 1024 * 1024]; // 分配10mb的缓存
        while (true)
        {
            int readCnt = stream.Read(buffer, 0, buffer.Length);

            if (readCnt == 0)
            {
                // tcp服务主动关闭了连接？
                break;
            }
        }
    }

    public void DisposeClient()
    {
        _client?.Close();
        _client?.Dispose();
        _client = null;
    }
}
