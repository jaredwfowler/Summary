using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Stock_Option_Trade_Analyzer
{
   public partial class LogIn : Form
   {
      private Form1 f = null;
      public LogIn(Form1 _f) 
      {
         InitializeComponent();
         this.f = _f;
      }
      public LogIn() {
         InitializeComponent();
      }
      private void button1_Click(object sender, EventArgs e) {
         if (f != null) {
            f.UserNameFidelity = this.textBox1.Text;
            f.PasswordFidelity = this.textBox2.Text;
            this.Close();
         }
      }
   }
}
