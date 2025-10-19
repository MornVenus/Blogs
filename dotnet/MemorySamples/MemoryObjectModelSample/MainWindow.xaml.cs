using System.Windows;

namespace MemoryObjectModelSample;

public class Student
{
    public int Id { get; set; }

    public string Name { get; set; }

    public Student(int id, string name)
    {
        Id = id;
        Name = name;
    }
}

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

    private void StringBtn_Click(object sender, RoutedEventArgs e)
    {
        string s = "";
        string str0 = "9";
        string str = "venus是我1234";
        string str2 = "56789";
        MessageBox.Show(str);
    }

    private void ClassBtn_Click(object sender, RoutedEventArgs e)
    {
        Student stu = new Student(1, "venus是我1234");
    }

    private void ArrayBtn_Click(object sender, RoutedEventArgs e)
    {
        int[] intArray = [1, 2, 3, 4, 5];
        string[] strArray = ["111", "222", "333", "444", "555"];
        Student[] stuArray = [new Student(1, "venus111"), new Student(2, "venus222"), new Student(3, "venus333"), new Student(4, "venus444")];
    }

    private void ListBtn_Click(object sender, RoutedEventArgs e)
    {
        List<int> intList = [1, 2, 3, 4, 5];
        List<string> strList = ["111", "222", "333", "444", "555"];
        List<Student> stuList = [new Student(1, "venus111"), new Student(2, "venus222"), new Student(3, "venus333"), new Student(4, "venus444")];
    }

    private void DictionaryBtn_Click(object sender, RoutedEventArgs e)
    {
        Dictionary<string, Student> dic = new Dictionary<string, Student>();
        dic.Add("111", new Student(1, "venus111"));
        dic.Add("222", new Student(2, "venus222"));
        dic.Add("333", new Student(3, "venus333"));
    }
}