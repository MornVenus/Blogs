using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace GraphicSample.Views;

public partial class RectMemView : UserControl
{
    public RectMemView()
    {
        InitializeComponent();
        this.DataContext = this;
    }

    public string Text
    {
        get { return (string)GetValue(TextProperty); }
        set { SetValue(TextProperty, value); }
    }

    // Using a DependencyProperty as the backing store for Text.  This enables animation, styling, binding, etc...
    public static readonly DependencyProperty TextProperty =
        DependencyProperty.Register("Text", typeof(string), typeof(RectMemView), new PropertyMetadata(""));

    public Brush BackBrush
    {
        get { return (Brush)GetValue(BackBrushProperty); }
        set { SetValue(BackBrushProperty, value); }
    }

    // Using a DependencyProperty as the backing store for BackBrush.  This enables animation, styling, binding, etc...
    public static readonly DependencyProperty BackBrushProperty =
        DependencyProperty.Register("BackBrush", typeof(Brush), typeof(RectMemView), new PropertyMetadata(default));
}
