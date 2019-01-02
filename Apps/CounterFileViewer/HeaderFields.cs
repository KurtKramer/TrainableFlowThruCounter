using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using CounterManaged;



namespace CounterFileViewer
{
  public partial class HeaderFields :Form
  {
    String  fileName = null;
    UmiScannerFile.HeaderField[]  headerFields = null;

    int  heightLast = 0;
    int  widthLast  = 0;

    public HeaderFields (String                        _fileName,
                         UmiScannerFile.HeaderField[]  _headerFields
                        )
    {
      fileName     = _fileName;
      headerFields = _headerFields;

      InitializeComponent ();

      heightLast = Height;
      widthLast  = Width;
    }


    private void HeaderFields_Load (object sender, EventArgs e)
    {
      FileName.Text = fileName;

      if  (headerFields == null)
        return;
      foreach  (UmiScannerFile.HeaderField  hf in headerFields)
      {
        String[]  fields = new String[2];
        fields[0] = hf.key;
        fields[1] = hf.value;
        ListViewItem  lvi = new ListViewItem (fields);
        HeaderFieldsDetails.Items.Add (lvi);
      }
    }

    private void HeaderFields_SizeChanged (object sender, EventArgs e)
    {
      int  deltaWidth  = Width  - widthLast;
      int  detlaHeight = Height - heightLast;

      FileName.Width += deltaWidth;
      HeaderFieldsDetails.Height += detlaHeight;
      HeaderFieldsDetails.Width  += deltaWidth;
    }
  }  /* HeaderFields */





}  /* CounterFileViewer */

